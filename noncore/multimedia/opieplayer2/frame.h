
#ifndef XINELIBFRAME_H
#define XINELIBFRAME_H

#include <xine.h>

namespace XINE {
    class Frame {
    public:
        Frame();
        Frame( int *width,  int* height,
               int *ratio_code, int *format,
               uint8_t **y, uint8_t **u,
               uint8_t **v );
        ~Frame();
        int* width() { return m_width; };
        int* height() { return m_height; };

        int* ratioCode() { return m_ratioCode; };
        int* format() { return m_format; };
        uint8_t ** y() { return m_y; };
        uint8_t ** u() { return m_u; };
        uint8_t ** v() { return m_v; };

        void setHeight( int* );
        void setWidth( int* );
        void setRatioCode(int *);
        void setFormat( int* );
        void setU( uint8_t** );
        void setY( uint8_t** );
        void setV( uint8_t** );

    private:
        friend class Lib;
        int* m_height;
        int* m_width;
        int* m_ratioCode;
        int* m_format;
        uint8_t** m_u;
        uint8_t** m_y;
        uint8_t** m_v;
    };
};

#endif
