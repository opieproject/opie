
#include "frame.h"

using namespace XINE;

Frame::Frame() {
    m_height = 0;
    m_width = 0;
    m_ratioCode = 0;
    m_format = 0;
    m_u = 0;
    m_y = 0;
    m_v = 0;
}
Frame::Frame( int *width,  int* height,
               int *ratio_code, int *format,
               uint8_t **y, uint8_t **u,
              uint8_t **v ) {
    m_width = width;
    m_height = height;
    m_ratioCode = ratio_code ;
    m_format = format;
    m_y = y;
    m_u = u;
    m_v = v;
}
Frame::~Frame() {


}
void Frame::setHeight( int* height ) {
    m_height = height;
}
void Frame::setWidth( int* width ) {
    m_width = width;
}
void Frame::setRatioCode( int* ratio ) {
    m_ratioCode = ratio;
}
void Frame::setFormat( int* format ) {
    m_format = format;
}
void Frame::setU( uint8_t** u ) {
    m_u = u;
}
void Frame::setY( uint8_t** y ) {
    m_y = y;
}
void Frame::setV( uint8_t** v ) {
    m_v = v;
}
