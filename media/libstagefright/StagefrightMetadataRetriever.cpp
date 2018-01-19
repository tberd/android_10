/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

//#define LOG_NDEBUG 0
#define LOG_TAG "StagefrightMetadataRetriever"

#include <inttypes.h>

#include <utils/Log.h>

#include "include/FrameDecoder.h"
#include "include/StagefrightMetadataRetriever.h"

#include <media/IMediaHTTPService.h>
#include <media/stagefright/foundation/ADebug.h>
#include <media/stagefright/foundation/AMessage.h>
#include <media/stagefright/DataSourceFactory.h>
#include <media/stagefright/FileSource.h>
#include <media/stagefright/MediaCodecList.h>
#include <media/stagefright/MediaDefs.h>
#include <media/stagefright/MediaErrors.h>
#include <media/stagefright/MediaExtractorFactory.h>
#include <media/stagefright/MetaData.h>
#include <media/CharacterEncodingDetector.h>

namespace android {

StagefrightMetadataRetriever::StagefrightMetadataRetriever()
    : mParsedMetaData(false),
      mAlbumArt(NULL) {
    ALOGV("StagefrightMetadataRetriever()");
}

StagefrightMetadataRetriever::~StagefrightMetadataRetriever() {
    ALOGV("~StagefrightMetadataRetriever()");
    clearMetadata();
    // Explicitly release extractor before continuing with the destructor,
    // some extractors might need to callback to close off the DataSource
    // and we need to make sure it's still there.
    if (mExtractor != NULL) {
        mExtractor->release();
    }
    if (mSource != NULL) {
        mSource->close();
    }
}

status_t StagefrightMetadataRetriever::setDataSource(
        const sp<IMediaHTTPService> &httpService,
        const char *uri,
        const KeyedVector<String8, String8> *headers) {
    ALOGV("setDataSource(%s)", uri);

    clearMetadata();
    mSource = DataSourceFactory::CreateFromURI(httpService, uri, headers);

    if (mSource == NULL) {
        ALOGE("Unable to create data source for '%s'.", uri);
        return UNKNOWN_ERROR;
    }

    mExtractor = MediaExtractorFactory::Create(mSource);

    if (mExtractor == NULL) {
        ALOGE("Unable to instantiate an extractor for '%s'.", uri);

        mSource.clear();

        return UNKNOWN_ERROR;
    }

    return OK;
}

// Warning caller retains ownership of the filedescriptor! Dup it if necessary.
status_t StagefrightMetadataRetriever::setDataSource(
        int fd, int64_t offset, int64_t length) {
    fd = dup(fd);

    ALOGV("setDataSource(%d, %" PRId64 ", %" PRId64 ")", fd, offset, length);

    clearMetadata();
    mSource = new FileSource(fd, offset, length);

    status_t err;
    if ((err = mSource->initCheck()) != OK) {
        mSource.clear();

        return err;
    }

    mExtractor = MediaExtractorFactory::Create(mSource);

    if (mExtractor == NULL) {
        mSource.clear();

        return UNKNOWN_ERROR;
    }

    return OK;
}

status_t StagefrightMetadataRetriever::setDataSource(
        const sp<DataSource>& source, const char *mime) {
    ALOGV("setDataSource(DataSource)");

    clearMetadata();
    mSource = source;
    mExtractor = MediaExtractorFactory::Create(mSource, mime);

    if (mExtractor == NULL) {
        ALOGE("Failed to instantiate a MediaExtractor.");
        mSource.clear();
        return UNKNOWN_ERROR;
    }

    return OK;
}

VideoFrame* StagefrightMetadataRetriever::getImageAtIndex(
        int index, int colorFormat, bool metaOnly) {

    ALOGV("getImageAtIndex: index: %d colorFormat: %d, metaOnly: %d",
            index, colorFormat, metaOnly);

    if (mExtractor.get() == NULL) {
        ALOGE("no extractor.");
        return NULL;
    }

    size_t n = mExtractor->countTracks();
    size_t i;
    int imageCount = 0;

    for (i = 0; i < n; ++i) {
        sp<MetaData> meta = mExtractor->getTrackMetaData(i);
        ALOGV("getting track %zu of %zu, meta=%s", i, n, meta->toString().c_str());

        const char *mime;
        CHECK(meta->findCString(kKeyMIMEType, &mime));

        if (!strncasecmp(mime, "image/", 6)) {
            int32_t isPrimary;
            if ((index < 0 && meta->findInt32(
                    kKeyTrackIsDefault, &isPrimary) && isPrimary)
                    || (index == imageCount++)) {
                break;
            }
        }
    }

    if (i == n) {
        ALOGE("image track not found.");
        return NULL;
    }

    sp<MetaData> trackMeta = mExtractor->getTrackMetaData(i);

    sp<IMediaSource> source = mExtractor->getTrack(i);

    if (source.get() == NULL) {
        ALOGE("unable to instantiate image track.");
        return NULL;
    }

    const char *mime;
    CHECK(trackMeta->findCString(kKeyMIMEType, &mime));
    ALOGV("extracting from %s track", mime);
    if (!strcasecmp(mime, MEDIA_MIMETYPE_IMAGE_ANDROID_HEIC)) {
        mime = MEDIA_MIMETYPE_VIDEO_HEVC;
        trackMeta = new MetaData(*trackMeta);
        trackMeta->setCString(kKeyMIMEType, mime);
    }

    Vector<AString> matchingCodecs;
    MediaCodecList::findMatchingCodecs(
            mime,
            false, /* encoder */
            MediaCodecList::kPreferSoftwareCodecs,
            &matchingCodecs);

    for (size_t i = 0; i < matchingCodecs.size(); ++i) {
        const AString &componentName = matchingCodecs[i];
        ImageDecoder decoder(componentName, trackMeta, source);
        VideoFrame* frame = decoder.extractFrame(
                0 /*frameTimeUs*/, 0 /*seekMode*/, colorFormat, metaOnly);

        if (frame != NULL) {
            return frame;
        }
        ALOGV("%s failed to extract thumbnail, trying next decoder.", componentName.c_str());
    }

    return NULL;
}

VideoFrame* StagefrightMetadataRetriever::getFrameAtTime(
        int64_t timeUs, int option, int colorFormat, bool metaOnly) {
    ALOGV("getFrameAtTime: %" PRId64 " us option: %d colorFormat: %d, metaOnly: %d",
            timeUs, option, colorFormat, metaOnly);

    VideoFrame *frame;
    status_t err = getFrameInternal(
            timeUs, 1, option, colorFormat, metaOnly, &frame, NULL /*outFrames*/);
    return (err == OK) ? frame : NULL;
}

status_t StagefrightMetadataRetriever::getFrameAtIndex(
        std::vector<VideoFrame*>* frames,
        int frameIndex, int numFrames, int colorFormat, bool metaOnly) {
    ALOGV("getFrameAtIndex: frameIndex %d, numFrames %d, colorFormat: %d, metaOnly: %d",
            frameIndex, numFrames, colorFormat, metaOnly);

    return getFrameInternal(
            frameIndex, numFrames, MediaSource::ReadOptions::SEEK_FRAME_INDEX,
            colorFormat, metaOnly, NULL /*outFrame*/, frames);
}

status_t StagefrightMetadataRetriever::getFrameInternal(
        int64_t timeUs, int numFrames, int option, int colorFormat, bool metaOnly,
        VideoFrame **outFrame, std::vector<VideoFrame*>* outFrames) {
    if (mExtractor.get() == NULL) {
        ALOGE("no extractor.");
        return NO_INIT;
    }

    sp<MetaData> fileMeta = mExtractor->getMetaData();

    if (fileMeta == NULL) {
        ALOGE("extractor doesn't publish metadata, failed to initialize?");
        return NO_INIT;
    }

    int32_t drm = 0;
    if (fileMeta->findInt32(kKeyIsDRM, &drm) && drm != 0) {
        ALOGE("frame grab not allowed.");
        return ERROR_DRM_UNKNOWN;
    }

    size_t n = mExtractor->countTracks();
    size_t i;
    for (i = 0; i < n; ++i) {
        sp<MetaData> meta = mExtractor->getTrackMetaData(i);

        const char *mime;
        CHECK(meta->findCString(kKeyMIMEType, &mime));

        if (!strncasecmp(mime, "video/", 6)) {
            break;
        }
    }

    if (i == n) {
        ALOGE("no video track found.");
        return INVALID_OPERATION;
    }

    sp<MetaData> trackMeta = mExtractor->getTrackMetaData(
            i, MediaExtractor::kIncludeExtensiveMetaData);

    sp<IMediaSource> source = mExtractor->getTrack(i);

    if (source.get() == NULL) {
        ALOGV("unable to instantiate video track.");
        return UNKNOWN_ERROR;
    }

    const void *data;
    uint32_t type;
    size_t dataSize;
    if (fileMeta->findData(kKeyAlbumArt, &type, &data, &dataSize)
            && mAlbumArt == NULL) {
        mAlbumArt = MediaAlbumArt::fromData(dataSize, data);
    }

    const char *mime;
    CHECK(trackMeta->findCString(kKeyMIMEType, &mime));

    Vector<AString> matchingCodecs;
    MediaCodecList::findMatchingCodecs(
            mime,
            false, /* encoder */
            MediaCodecList::kPreferSoftwareCodecs,
            &matchingCodecs);

    for (size_t i = 0; i < matchingCodecs.size(); ++i) {
        const AString &componentName = matchingCodecs[i];
        VideoFrameDecoder decoder(componentName, trackMeta, source);
        if (outFrame != NULL) {
            *outFrame = decoder.extractFrame(
                    timeUs, option, colorFormat, metaOnly);
            if (*outFrame != NULL) {
                return OK;
            }
        } else if (outFrames != NULL) {
            status_t err = decoder.extractFrames(
                    timeUs, numFrames, option, colorFormat, outFrames);
            if (err == OK) {
                return OK;
            }
        }
        ALOGV("%s failed to extract frame, trying next decoder.", componentName.c_str());
    }

    ALOGE("all codecs failed to extract frame.");
    return UNKNOWN_ERROR;
}

MediaAlbumArt *StagefrightMetadataRetriever::extractAlbumArt() {
    ALOGV("extractAlbumArt (extractor: %s)", mExtractor.get() != NULL ? "YES" : "NO");

    if (mExtractor == NULL) {
        return NULL;
    }

    if (!mParsedMetaData) {
        parseMetaData();

        mParsedMetaData = true;
    }

    if (mAlbumArt) {
        return mAlbumArt->clone();
    }

    return NULL;
}

const char *StagefrightMetadataRetriever::extractMetadata(int keyCode) {
    if (mExtractor == NULL) {
        return NULL;
    }

    if (!mParsedMetaData) {
        parseMetaData();

        mParsedMetaData = true;
    }

    ssize_t index = mMetaData.indexOfKey(keyCode);

    if (index < 0) {
        return NULL;
    }

    return mMetaData.valueAt(index).string();
}

void StagefrightMetadataRetriever::parseMetaData() {
    sp<MetaData> meta = mExtractor->getMetaData();

    if (meta == NULL) {
        ALOGV("extractor doesn't publish metadata, failed to initialize?");
        return;
    }

    struct Map {
        int from;
        int to;
        const char *name;
    };
    static const Map kMap[] = {
        { kKeyMIMEType, METADATA_KEY_MIMETYPE, NULL },
        { kKeyCDTrackNumber, METADATA_KEY_CD_TRACK_NUMBER, "tracknumber" },
        { kKeyDiscNumber, METADATA_KEY_DISC_NUMBER, "discnumber" },
        { kKeyAlbum, METADATA_KEY_ALBUM, "album" },
        { kKeyArtist, METADATA_KEY_ARTIST, "artist" },
        { kKeyAlbumArtist, METADATA_KEY_ALBUMARTIST, "albumartist" },
        { kKeyAuthor, METADATA_KEY_AUTHOR, NULL },
        { kKeyComposer, METADATA_KEY_COMPOSER, "composer" },
        { kKeyDate, METADATA_KEY_DATE, NULL },
        { kKeyGenre, METADATA_KEY_GENRE, "genre" },
        { kKeyTitle, METADATA_KEY_TITLE, "title" },
        { kKeyYear, METADATA_KEY_YEAR, "year" },
        { kKeyWriter, METADATA_KEY_WRITER, "writer" },
        { kKeyCompilation, METADATA_KEY_COMPILATION, "compilation" },
        { kKeyLocation, METADATA_KEY_LOCATION, NULL },
    };

    static const size_t kNumMapEntries = sizeof(kMap) / sizeof(kMap[0]);

    CharacterEncodingDetector *detector = new CharacterEncodingDetector();

    for (size_t i = 0; i < kNumMapEntries; ++i) {
        const char *value;
        if (meta->findCString(kMap[i].from, &value)) {
            if (kMap[i].name) {
                // add to charset detector
                detector->addTag(kMap[i].name, value);
            } else {
                // directly add to output list
                mMetaData.add(kMap[i].to, String8(value));
            }
        }
    }

    detector->detectAndConvert();
    int size = detector->size();
    if (size) {
        for (int i = 0; i < size; i++) {
            const char *name;
            const char *value;
            detector->getTag(i, &name, &value);
            for (size_t j = 0; j < kNumMapEntries; ++j) {
                if (kMap[j].name && !strcmp(kMap[j].name, name)) {
                    mMetaData.add(kMap[j].to, String8(value));
                }
            }
        }
    }
    delete detector;

    const void *data;
    uint32_t type;
    size_t dataSize;
    if (meta->findData(kKeyAlbumArt, &type, &data, &dataSize)
            && mAlbumArt == NULL) {
        mAlbumArt = MediaAlbumArt::fromData(dataSize, data);
    }

    size_t numTracks = mExtractor->countTracks();

    char tmp[32];
    sprintf(tmp, "%zu", numTracks);

    mMetaData.add(METADATA_KEY_NUM_TRACKS, String8(tmp));

    float captureFps;
    if (meta->findFloat(kKeyCaptureFramerate, &captureFps)) {
        sprintf(tmp, "%f", captureFps);
        mMetaData.add(METADATA_KEY_CAPTURE_FRAMERATE, String8(tmp));
    }

    bool hasAudio = false;
    bool hasVideo = false;
    int32_t videoWidth = -1;
    int32_t videoHeight = -1;
    int32_t videoFrameCount = 0;
    int32_t audioBitrate = -1;
    int32_t rotationAngle = -1;
    int32_t imageCount = 0;
    int32_t imagePrimary = 0;
    int32_t imageWidth = -1;
    int32_t imageHeight = -1;
    int32_t imageRotation = -1;

    // The overall duration is the duration of the longest track.
    int64_t maxDurationUs = 0;
    String8 timedTextLang;
    for (size_t i = 0; i < numTracks; ++i) {
        sp<MetaData> trackMeta = mExtractor->getTrackMetaData(i);

        int64_t durationUs;
        if (trackMeta->findInt64(kKeyDuration, &durationUs)) {
            if (durationUs > maxDurationUs) {
                maxDurationUs = durationUs;
            }
        }

        const char *mime;
        if (trackMeta->findCString(kKeyMIMEType, &mime)) {
            if (!hasAudio && !strncasecmp("audio/", mime, 6)) {
                hasAudio = true;

                if (!trackMeta->findInt32(kKeyBitRate, &audioBitrate)) {
                    audioBitrate = -1;
                }
            } else if (!hasVideo && !strncasecmp("video/", mime, 6)) {
                hasVideo = true;

                CHECK(trackMeta->findInt32(kKeyWidth, &videoWidth));
                CHECK(trackMeta->findInt32(kKeyHeight, &videoHeight));
                if (!trackMeta->findInt32(kKeyRotation, &rotationAngle)) {
                    rotationAngle = 0;
                }
                if (!trackMeta->findInt32(kKeyFrameCount, &videoFrameCount)) {
                    videoFrameCount = 0;
                }
            } else if (!strncasecmp("image/", mime, 6)) {
                int32_t isPrimary;
                if (trackMeta->findInt32(
                        kKeyTrackIsDefault, &isPrimary) && isPrimary) {
                    imagePrimary = imageCount;
                    CHECK(trackMeta->findInt32(kKeyWidth, &imageWidth));
                    CHECK(trackMeta->findInt32(kKeyHeight, &imageHeight));
                    if (!trackMeta->findInt32(kKeyRotation, &imageRotation)) {
                        imageRotation = 0;
                    }
                }
                imageCount++;
            } else if (!strcasecmp(mime, MEDIA_MIMETYPE_TEXT_3GPP)) {
                const char *lang;
                if (trackMeta->findCString(kKeyMediaLanguage, &lang)) {
                    timedTextLang.append(String8(lang));
                    timedTextLang.append(String8(":"));
                } else {
                    ALOGE("No language found for timed text");
                }
            }
        }
    }

    // To save the language codes for all timed text tracks
    // If multiple text tracks present, the format will look
    // like "eng:chi"
    if (!timedTextLang.isEmpty()) {
        mMetaData.add(METADATA_KEY_TIMED_TEXT_LANGUAGES, timedTextLang);
    }

    // The duration value is a string representing the duration in ms.
    sprintf(tmp, "%" PRId64, (maxDurationUs + 500) / 1000);
    mMetaData.add(METADATA_KEY_DURATION, String8(tmp));

    if (hasAudio) {
        mMetaData.add(METADATA_KEY_HAS_AUDIO, String8("yes"));
    }

    if (hasVideo) {
        mMetaData.add(METADATA_KEY_HAS_VIDEO, String8("yes"));

        sprintf(tmp, "%d", videoWidth);
        mMetaData.add(METADATA_KEY_VIDEO_WIDTH, String8(tmp));

        sprintf(tmp, "%d", videoHeight);
        mMetaData.add(METADATA_KEY_VIDEO_HEIGHT, String8(tmp));

        sprintf(tmp, "%d", rotationAngle);
        mMetaData.add(METADATA_KEY_VIDEO_ROTATION, String8(tmp));

        if (videoFrameCount > 0) {
            sprintf(tmp, "%d", videoFrameCount);
            mMetaData.add(METADATA_KEY_VIDEO_FRAME_COUNT, String8(tmp));
        }
    }

    if (imageCount > 0) {
        mMetaData.add(METADATA_KEY_HAS_IMAGE, String8("yes"));

        sprintf(tmp, "%d", imageCount);
        mMetaData.add(METADATA_KEY_IMAGE_COUNT, String8(tmp));

        sprintf(tmp, "%d", imagePrimary);
        mMetaData.add(METADATA_KEY_IMAGE_PRIMARY, String8(tmp));

        sprintf(tmp, "%d", imageWidth);
        mMetaData.add(METADATA_KEY_IMAGE_WIDTH, String8(tmp));

        sprintf(tmp, "%d", imageHeight);
        mMetaData.add(METADATA_KEY_IMAGE_HEIGHT, String8(tmp));

        sprintf(tmp, "%d", imageRotation);
        mMetaData.add(METADATA_KEY_IMAGE_ROTATION, String8(tmp));
    }

    if (numTracks == 1 && hasAudio && audioBitrate >= 0) {
        sprintf(tmp, "%d", audioBitrate);
        mMetaData.add(METADATA_KEY_BITRATE, String8(tmp));
    } else {
        off64_t sourceSize;
        if (mSource != NULL && mSource->getSize(&sourceSize) == OK) {
            int64_t avgBitRate = (int64_t)(sourceSize * 8E6 / maxDurationUs);

            sprintf(tmp, "%" PRId64, avgBitRate);
            mMetaData.add(METADATA_KEY_BITRATE, String8(tmp));
        }
    }

    if (numTracks == 1) {
        const char *fileMIME;

        if (meta->findCString(kKeyMIMEType, &fileMIME) &&
                !strcasecmp(fileMIME, "video/x-matroska")) {
            sp<MetaData> trackMeta = mExtractor->getTrackMetaData(0);
            const char *trackMIME;
            CHECK(trackMeta->findCString(kKeyMIMEType, &trackMIME));

            if (!strncasecmp("audio/", trackMIME, 6)) {
                // The matroska file only contains a single audio track,
                // rewrite its mime type.
                mMetaData.add(
                        METADATA_KEY_MIMETYPE, String8("audio/x-matroska"));
            }
        }
    }
}

void StagefrightMetadataRetriever::clearMetadata() {
    mParsedMetaData = false;
    mMetaData.clear();
    delete mAlbumArt;
    mAlbumArt = NULL;
}

}  // namespace android