
#ifndef MEDIADETECT_H
#define MEDIADETECT_H


class QString;
class MediaDetect {

public:
    MediaDetect();
    ~MediaDetect();

    /**
     * Look at the filename and decide which gui is to be used
     *
     * @param filename the filename
     * @return the char - a for audio gui, v for video, f for error
     */
    char videoOrAudio( const QString& fileName );

    /**
     * Find out if it is a streaming media file
     *
     * @param filename the filename
     * @return if it is a streaming url or not
     */
    bool isStreaming( const QString& fileName );

};

#endif
