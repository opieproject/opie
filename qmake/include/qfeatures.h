// All feature and their dependencies
//
// This list is generated from $QTDIR/src/tools/qfeatures.txt
//
// Asynchronous I/O
//#define QT_NO_ASYNC_IO

// Bezier curves
//#define QT_NO_BEZIER

// Buttons
//#define QT_NO_BUTTON

// Named colors
//#define QT_NO_COLORNAMES

// Cursors
//#define QT_NO_CURSOR

// QDataStream
//#define QT_NO_DATASTREAM

// QDate/QTime/QDateTime toString() and fromString()
//#define QT_NO_DATESTRING

// Dialogs
//#define QT_NO_DIALOG

// QDirectPainter
//#define QT_NO_DIRECTPAINTER

// Special widget effects (fading, scrolling)
//#define QT_NO_EFFECTS

// Freetype font engine
//#define QT_NO_FREETYPE

// Dither QImage to 1-bit image
//#define QT_NO_IMAGE_DITHER_TO_1

// QImage::createHeuristicMask()
//#define QT_NO_IMAGE_HEURISTIC_MASK

// QImage mirroring
//#define QT_NO_IMAGE_MIRROR

// Smooth QImage scaling
//#define QT_NO_IMAGE_SMOOTHSCALE

// TrueColor QImage
//#define QT_NO_IMAGE_TRUECOLOR

// Automatic widget layout
//#define QT_NO_LAYOUT

// Networking
//#define QT_NO_NETWORK

// Palettes
//#define QT_NO_PALETTE

// Alpha-blended cursor
//#define QT_NO_QWS_ALPHA_CURSOR

// 1-bit monochrome
//#define QT_NO_QWS_DEPTH_1

// 15 or 16-bit color
//#define QT_NO_QWS_DEPTH_16

// 24-bit color
//#define QT_NO_QWS_DEPTH_24

// 32-bit color
//#define QT_NO_QWS_DEPTH_32

// 4-bit greyscale
//#define QT_NO_QWS_DEPTH_4

// 8-bit color
//#define QT_NO_QWS_DEPTH_8

// 8-bit grayscale
//#define QT_NO_QWS_DEPTH_8GRAYSCALE

// Favour code size over graphics speed
//#define QT_NO_QWS_GFX_SPEED

// Console keyboard
//#define QT_NO_QWS_KEYBOARD

// Linux framebuffer
//#define QT_NO_QWS_LINUXFB

// Mach64 acceleration
//#define QT_NO_QWS_MACH64

// Window Manager
//#define QT_NO_QWS_MANAGER

// Matrox MGA acceleration
//#define QT_NO_QWS_MATROX

// Autodetecting mouse driver
//#define QT_NO_QWS_MOUSE_AUTO

// Non-autodetecting mouse driver
//#define QT_NO_QWS_MOUSE_MANUAL

// Qt/Embedded window system properties.
//#define QT_NO_QWS_PROPERTIES

// Repeater display
//#define QT_NO_QWS_REPEATER

// Saving of fonts
//#define QT_NO_QWS_SAVEFONTS

// Shadow frame buffer
//#define QT_NO_QWS_SHADOWFB

// Virtual frame buffer
//#define QT_NO_QWS_VFB

// 4-bit VGA
//#define QT_NO_QWS_VGA_16

// Voodoo3 acceleration
//#define QT_NO_QWS_VOODOO3

// Range-control widgets
//#define QT_NO_RANGECONTROL

// Regular expression capture
//#define QT_NO_REGEXP

// QSignalMapper
//#define QT_NO_SIGNALMAPPER

// Playing sounds
//#define QT_NO_SOUND

// Standard template library compatiblity
//#define QT_NO_STL

// QStringList
//#define QT_NO_STRINGLIST

// Character set conversions
//#define QT_NO_TEXTCODEC

// QTextStream
//#define QT_NO_TEXTSTREAM

// Unicode property tables
//#define QT_NO_UNICODETABLES

// Input validators
//#define QT_NO_VALIDATOR

// QVariant
//#define QT_NO_VARIANT

// Wheel-mouse events
//#define QT_NO_WHEELEVENT

// QWMatrix
//#define QT_NO_WMATRIX

// Non-Unicode text conversions
#if !defined(QT_NO_CODECS) && (defined(QT_NO_TEXTCODEC))
#define QT_NO_CODECS
#endif

// QCop IPC
#if !defined(QT_NO_COP) && (defined(QT_NO_DATASTREAM))
#define QT_NO_COP
#endif

// QFontDatabase
#if !defined(QT_NO_FONTDATABASE) && (defined(QT_NO_STRINGLIST))
#define QT_NO_FONTDATABASE
#endif

// Image formats
#if !defined(QT_NO_IMAGEIO) && (defined(QT_NO_REGEXP))
#define QT_NO_IMAGEIO
#endif

// 16-bit QImage
#if !defined(QT_NO_IMAGE_16_BIT) && (defined(QT_NO_IMAGE_TRUECOLOR))
#define QT_NO_IMAGE_16_BIT
#endif

// Image file text strings
#if !defined(QT_NO_IMAGE_TEXT) && (defined(QT_NO_STRINGLIST))
#define QT_NO_IMAGE_TEXT
#endif

// Shared library wrapper
#if !defined(QT_NO_LIBRARY) && (defined(QT_NO_REGEXP))
#define QT_NO_LIBRARY
#endif

// Pixmap transformations
#if !defined(QT_NO_PIXMAP_TRANSFORMATION) && (defined(QT_NO_WMATRIX))
#define QT_NO_PIXMAP_TRANSFORMATION
#endif

// Convert UUID to/from string
#if !defined(QT_NO_QUUID_STRING) && (defined(QT_NO_STRINGLIST))
#define QT_NO_QUUID_STRING
#endif

// The "BeOS" style
#if !defined(QT_NO_QWS_BEOS_WM_STYLE) && (defined(QT_NO_QWS_MANAGER))
#define QT_NO_QWS_BEOS_WM_STYLE
#endif

// Visible cursor
#if !defined(QT_NO_QWS_CURSOR) && (defined(QT_NO_CURSOR))
#define QT_NO_QWS_CURSOR
#endif

// 32-bit color, BGR order
#if !defined(QT_NO_QWS_DEPTH_32_BGR) && (defined(QT_NO_QWS_DEPTH_32))
#define QT_NO_QWS_DEPTH_32_BGR
#endif

// The "Hydro" style
#if !defined(QT_NO_QWS_HYDRO_WM_STYLE) && (defined(QT_NO_QWS_MANAGER))
#define QT_NO_QWS_HYDRO_WM_STYLE
#endif

// The "KDE2" style
#if !defined(QT_NO_QWS_KDE2_WM_STYLE) && (defined(QT_NO_QWS_MANAGER))
#define QT_NO_QWS_KDE2_WM_STYLE
#endif

// The "KDE" style
#if !defined(QT_NO_QWS_KDE_WM_STYLE) && (defined(QT_NO_QWS_MANAGER))
#define QT_NO_QWS_KDE_WM_STYLE
#endif

// Multi-process architecture
#if !defined(QT_NO_QWS_MULTIPROCESS) && (defined(QT_NO_NETWORK))
#define QT_NO_QWS_MULTIPROCESS
#endif

// Transformed frame buffer
#if !defined(QT_NO_QWS_TRANSFORMED) && (defined(QT_NO_QWS_LINUXFB))
#define QT_NO_QWS_TRANSFORMED
#endif

// Remote frame buffer (VNC)
#if !defined(QT_NO_QWS_VNC) && (defined(QT_NO_NETWORK))
#define QT_NO_QWS_VNC
#endif

// The "Windows" style
#if !defined(QT_NO_QWS_WINDOWS_WM_STYLE) && (defined(QT_NO_QWS_MANAGER))
#define QT_NO_QWS_WINDOWS_WM_STYLE
#endif

// Regular expression anchors
#if !defined(QT_NO_REGEXP_ANCHOR_ALT) && (defined(QT_NO_REGEXP))
#define QT_NO_REGEXP_ANCHOR_ALT
#endif

// Regular expression back-reference
#if !defined(QT_NO_REGEXP_BACKREF) && (defined(QT_NO_REGEXP))
#define QT_NO_REGEXP_BACKREF
#endif

// Regular expression character-class
#if !defined(QT_NO_REGEXP_CCLASS) && (defined(QT_NO_REGEXP))
#define QT_NO_REGEXP_CCLASS
#endif

// Regular expression escape
#if !defined(QT_NO_REGEXP_ESCAPE) && (defined(QT_NO_REGEXP))
#define QT_NO_REGEXP_ESCAPE
#endif

// Regular expression interval
#if !defined(QT_NO_REGEXP_INTERVAL) && (defined(QT_NO_REGEXP))
#define QT_NO_REGEXP_INTERVAL
#endif

// Regular expression lookahead
#if !defined(QT_NO_REGEXP_LOOKAHEAD) && (defined(QT_NO_REGEXP))
#define QT_NO_REGEXP_LOOKAHEAD
#endif

// Regular expression optimization
#if !defined(QT_NO_REGEXP_OPTIM) && (defined(QT_NO_REGEXP))
#define QT_NO_REGEXP_OPTIM
#endif

// Regular expression wildcard
#if !defined(QT_NO_REGEXP_WILDCARD) && (defined(QT_NO_REGEXP))
#define QT_NO_REGEXP_WILDCARD
#endif

// Semi-modal dialogs
#if !defined(QT_NO_SEMIMODAL) && (defined(QT_NO_DIALOG))
#define QT_NO_SEMIMODAL
#endif

// Session management
#if !defined(QT_NO_SESSIONMANAGER) && (defined(QT_NO_STRINGLIST))
#define QT_NO_SESSIONMANAGER
#endif

// QString::sprintf()
#if !defined(QT_NO_SPRINTF) && (defined(QT_NO_REGEXP))
#define QT_NO_SPRINTF
#endif

// Scaling and rotation
#if !defined(QT_NO_TRANSFORMATIONS) && (defined(QT_NO_WMATRIX))
#define QT_NO_TRANSFORMATIONS
#endif

// Translations via QObject::tr()
#if !defined(QT_NO_TRANSLATION) && (defined(QT_NO_DATASTREAM))
#define QT_NO_TRANSLATION
#endif

// Window icon and caption
#if !defined(QT_NO_WIDGET_TOPEXTRA) && (defined(QT_NO_IMAGE_HEURISTIC_MASK))
#define QT_NO_WIDGET_TOPEXTRA
#endif

// Keyboard accelerators and shortcuts
#if !defined(QT_NO_ACCEL) && (defined(QT_NO_SPRINTF))
#define QT_NO_ACCEL
#endif

// Asynchronous image I/O
#if !defined(QT_NO_ASYNC_IMAGE_IO) && (defined(QT_NO_IMAGEIO))
#define QT_NO_ASYNC_IMAGE_IO
#endif

// BDF font files
#if !defined(QT_NO_BDF) && (defined(QT_NO_TEXTSTREAM) || defined(QT_NO_STRINGLIST))
#define QT_NO_BDF
#endif

// QDir
#if !defined(QT_NO_DIR) && (defined(QT_NO_STRINGLIST) || defined(QT_NO_REGEXP))
#define QT_NO_DIR
#endif

// JPEG image I/O
#if !defined(QT_NO_IMAGEIO_JPEG) && (defined(QT_NO_IMAGEIO))
#define QT_NO_IMAGEIO_JPEG
#endif

// MNG image I/O
#if !defined(QT_NO_IMAGEIO_MNG) && (defined(QT_NO_IMAGEIO))
#define QT_NO_IMAGEIO_MNG
#endif

// PNG image I/O
#if !defined(QT_NO_IMAGEIO_PNG) && (defined(QT_NO_IMAGEIO))
#define QT_NO_IMAGEIO_PNG
#endif

// PPM image I/O
#if !defined(QT_NO_IMAGEIO_PPM) && (defined(QT_NO_IMAGEIO))
#define QT_NO_IMAGEIO_PPM
#endif

// XBM image I/O
#if !defined(QT_NO_IMAGEIO_XBM) && (defined(QT_NO_IMAGEIO))
#define QT_NO_IMAGEIO_XBM
#endif

// Image transformations
#if !defined(QT_NO_IMAGE_TRANSFORMATION) && (defined(QT_NO_PIXMAP_TRANSFORMATION))
#define QT_NO_IMAGE_TRANSFORMATION
#endif

// External process invocation.
#if !defined(QT_NO_PROCESS) && (defined(QT_NO_STRINGLIST) || defined(QT_NO_REGEXP))
#define QT_NO_PROCESS
#endif

// Regular expression capture
#if !defined(QT_NO_REGEXP_CAPTURE) && (defined(QT_NO_REGEXP) || defined(QT_NO_STRINGLIST))
#define QT_NO_REGEXP_CAPTURE
#endif

// Template classes in QVariant
#if !defined(QT_NO_TEMPLATE_VARIANT) && (defined(QT_NO_VARIANT) || defined(QT_NO_STRINGLIST))
#define QT_NO_TEMPLATE_VARIANT
#endif

// Month and day names in dates
#if !defined(QT_NO_TEXTDATE) && (defined(QT_NO_STRINGLIST) || defined(QT_NO_DATESTRING))
#define QT_NO_TEXTDATE
#endif

// Drawing utility functions
#if !defined(QT_NO_DRAWUTIL) && (defined(QT_NO_SPRINTF) || defined(QT_NO_PALETTE))
#define QT_NO_DRAWUTIL
#endif

// BMP image I/O
#if !defined(QT_NO_IMAGEIO_BMP) && (defined(QT_NO_IMAGEIO) || defined(QT_NO_DATASTREAM))
#define QT_NO_IMAGEIO_BMP
#endif

// QPicture
#if !defined(QT_NO_PICTURE) && (defined(QT_NO_DATASTREAM) || defined(QT_NO_IMAGEIO))
#define QT_NO_PICTURE
#endif

// Translations via QObject::trUtf8()
#if !defined(QT_NO_TRANSLATION_UTF8) && (defined(QT_NO_TRANSLATION) || defined(QT_NO_TEXTCODEC))
#define QT_NO_TRANSLATION_UTF8
#endif

// URL parser
#if !defined(QT_NO_URL) && (defined(QT_NO_DIR))
#define QT_NO_URL
#endif

// Animated images
#if !defined(QT_NO_MOVIE) && (defined(QT_NO_ASYNC_IO) || defined(QT_NO_ASYNC_IMAGE_IO))
#define QT_NO_MOVIE
#endif

// Persistent application settings
#if !defined(QT_NO_SETTINGS) && (defined(QT_NO_DIR) || defined(QT_NO_TEXTSTREAM))
#define QT_NO_SETTINGS
#endif

// QStyle
#if !defined(QT_NO_STYLE) && (defined(QT_NO_DRAWUTIL))
#define QT_NO_STYLE
#endif

// DNS
#if !defined(QT_NO_DNS) && (defined(QT_NO_NETWORK) || defined(QT_NO_STRINGLIST) || defined(QT_NO_TEXTSTREAM) || defined(QT_NO_SPRINTF))
#define QT_NO_DNS
#endif

// Framed widgets
#if !defined(QT_NO_FRAME) && (defined(QT_NO_STYLE))
#define QT_NO_FRAME
#endif

// QIconSet
#if !defined(QT_NO_ICONSET) && (defined(QT_NO_IMAGEIO) || defined(QT_NO_IMAGE_SMOOTHSCALE) || defined(QT_NO_PALETTE) || defined(QT_NO_IMAGE_HEURISTIC_MASK))
#define QT_NO_ICONSET
#endif

// XPM image I/O
#if !defined(QT_NO_IMAGEIO_XPM) && (defined(QT_NO_IMAGEIO) || defined(QT_NO_SPRINTF) || defined(QT_NO_TEXTSTREAM))
#define QT_NO_IMAGEIO_XPM
#endif

// QSizeGrip
#if !defined(QT_NO_SIZEGRIP) && (defined(QT_NO_STYLE))
#define QT_NO_SIZEGRIP
#endif

// Motif style
#if !defined(QT_NO_STYLE_MOTIF) && (defined(QT_NO_STYLE))
#define QT_NO_STYLE_MOTIF
#endif

// Windows style
#if !defined(QT_NO_STYLE_WINDOWS) && (defined(QT_NO_STYLE))
#define QT_NO_STYLE_WINDOWS
#endif

// Internal titlebar widget
#if !defined(QT_NO_TITLEBAR) && (defined(QT_NO_STYLE))
#define QT_NO_TITLEBAR
#endif

// XML
#if !defined(QT_NO_XML) && (defined(QT_NO_TEXTSTREAM) || defined(QT_NO_TEXTCODEC) || defined(QT_NO_REGEXP_CAPTURE))
#define QT_NO_XML
#endif

// Check-boxes
#if !defined(QT_NO_CHECKBOX) && (defined(QT_NO_BUTTON) || defined(QT_NO_STYLE))
#define QT_NO_CHECKBOX
#endif

// Dials
#if !defined(QT_NO_DIAL) && (defined(QT_NO_RANGECONTROL) || defined(QT_NO_STYLE))
#define QT_NO_DIAL
#endif

// QLabel
#if !defined(QT_NO_LABEL) && (defined(QT_NO_FRAME))
#define QT_NO_LABEL
#endif

// QLCDNumber
#if !defined(QT_NO_LCDNUMBER) && (defined(QT_NO_FRAME))
#define QT_NO_LCDNUMBER
#endif

// MIME
#if !defined(QT_NO_MIME) && (defined(QT_NO_DIR) || defined(QT_NO_IMAGEIO) || defined(QT_NO_TEXTCODEC))
#define QT_NO_MIME
#endif

// Printing
#if !defined(QT_NO_PRINTER) && (defined(QT_NO_TEXTSTREAM) || defined(QT_NO_SPRINTF) || defined(QT_NO_FONTDATABASE) || defined(QT_NO_DATESTRING))
#define QT_NO_PRINTER
#endif

// Progress bars
#if !defined(QT_NO_PROGRESSBAR) && (defined(QT_NO_FRAME))
#define QT_NO_PROGRESSBAR
#endif

// Radio-buttons
#if !defined(QT_NO_RADIOBUTTON) && (defined(QT_NO_BUTTON) || defined(QT_NO_STYLE))
#define QT_NO_RADIOBUTTON
#endif

// Internal resize handler
#if !defined(QT_NO_RESIZEHANDLER) && (defined(QT_NO_FRAME))
#define QT_NO_RESIZEHANDLER
#endif

// Scroll bars
#if !defined(QT_NO_SCROLLBAR) && (defined(QT_NO_RANGECONTROL) || defined(QT_NO_STYLE))
#define QT_NO_SCROLLBAR
#endif

// Sliders
#if !defined(QT_NO_SLIDER) && (defined(QT_NO_RANGECONTROL) || defined(QT_NO_STYLE))
#define QT_NO_SLIDER
#endif

// Spinbox control widget
#if !defined(QT_NO_SPINWIDGET) && (defined(QT_NO_FRAME))
#define QT_NO_SPINWIDGET
#endif

// Status bars
#if !defined(QT_NO_STATUSBAR) && (defined(QT_NO_LAYOUT) || defined(QT_NO_STYLE))
#define QT_NO_STATUSBAR
#endif

// Compact Windows style
#if !defined(QT_NO_STYLE_COMPACT) && (defined(QT_NO_STYLE_WINDOWS))
#define QT_NO_STYLE_COMPACT
#endif

// Interlace-friendly style
#if !defined(QT_NO_STYLE_INTERLACE) && (defined(QT_NO_STYLE_MOTIF))
#define QT_NO_STYLE_INTERLACE
#endif

// Platinum style
#if !defined(QT_NO_STYLE_PLATINUM) && (defined(QT_NO_STYLE_WINDOWS))
#define QT_NO_STYLE_PLATINUM
#endif

// Widget stacks
#if !defined(QT_NO_WIDGETSTACK) && (defined(QT_NO_FRAME))
#define QT_NO_WIDGETSTACK
#endif

// Grid layout widgets
#if !defined(QT_NO_GRID) && (defined(QT_NO_LAYOUT) || defined(QT_NO_FRAME))
#define QT_NO_GRID
#endif

// Group boxes
#if !defined(QT_NO_GROUPBOX) && (defined(QT_NO_FRAME) || defined(QT_NO_LAYOUT))
#define QT_NO_GROUPBOX
#endif

// Horizonal box layout widgets
#if !defined(QT_NO_HBOX) && (defined(QT_NO_LAYOUT) || defined(QT_NO_FRAME))
#define QT_NO_HBOX
#endif

// Menu-oriented widgets
#if !defined(QT_NO_MENUDATA) && (defined(QT_NO_ICONSET) || defined(QT_NO_VARIANT))
#define QT_NO_MENUDATA
#endif

// RichText (HTML) display
#if !defined(QT_NO_RICHTEXT) && (defined(QT_NO_DRAWUTIL) || defined(QT_NO_LAYOUT) || defined(QT_NO_STRINGLIST) || defined(QT_NO_TEXTSTREAM))
#define QT_NO_RICHTEXT
#endif

// Splitters
#if !defined(QT_NO_SPLITTER) && (defined(QT_NO_FRAME) || defined(QT_NO_LAYOUT))
#define QT_NO_SPLITTER
#endif

// Table-like widgets
#if !defined(QT_NO_TABLEVIEW) && (defined(QT_NO_SCROLLBAR))
#define QT_NO_TABLEVIEW
#endif

// Tool tips
#if !defined(QT_NO_TOOLTIP) && (defined(QT_NO_LABEL))
#define QT_NO_TOOLTIP
#endif

// Cut and paste
#if !defined(QT_NO_CLIPBOARD) && (defined(QT_NO_QWS_PROPERTIES) || defined(QT_NO_MIME))
#define QT_NO_CLIPBOARD
#endif

// Complex scripts (eg. BiDi)
#if !defined(QT_NO_COMPLEXTEXT) && (defined(QT_NO_RICHTEXT))
#define QT_NO_COMPLEXTEXT
#endif

// Horizontal group boxes
#if !defined(QT_NO_HGROUPBOX) && (defined(QT_NO_GROUPBOX))
#define QT_NO_HGROUPBOX
#endif

// Network file access
#if !defined(QT_NO_NETWORKPROTOCOL) && (defined(QT_NO_DIR) || defined(QT_NO_TEXTCODEC) || defined(QT_NO_URL))
#define QT_NO_NETWORKPROTOCOL
#endif

// Properties
#if !defined(QT_NO_PROPERTIES) && (defined(QT_NO_VARIANT) || defined(QT_NO_STRINGLIST) || defined(QT_NO_ICONSET))
#define QT_NO_PROPERTIES
#endif

// CDE style
#if !defined(QT_NO_STYLE_CDE) && (defined(QT_NO_STYLE_MOTIF) || defined(QT_NO_TRANSFORMATIONS))
#define QT_NO_STYLE_CDE
#endif

// Motif-plus style
#if !defined(QT_NO_STYLE_MOTIFPLUS) && (defined(QT_NO_STYLE_MOTIF) || defined(QT_NO_TRANSFORMATIONS))
#define QT_NO_STYLE_MOTIFPLUS
#endif

// SGI style
#if !defined(QT_NO_STYLE_SGI) && (defined(QT_NO_STYLE_MOTIF) || defined(QT_NO_TRANSFORMATIONS))
#define QT_NO_STYLE_SGI
#endif

// Vertical box layout widgets
#if !defined(QT_NO_VBOX) && (defined(QT_NO_HBOX))
#define QT_NO_VBOX
#endif

// Button groups
#if !defined(QT_NO_BUTTONGROUP) && (defined(QT_NO_GROUPBOX) || defined(QT_NO_BUTTON))
#define QT_NO_BUTTONGROUP
#endif

// Cut and paste non-text
#if !defined(QT_NO_MIMECLIPBOARD) && (defined(QT_NO_CLIPBOARD))
#define QT_NO_MIMECLIPBOARD
#endif

// Aqua style
#if !defined(QT_NO_STYLE_AQUA) && (defined(QT_NO_STYLE_WINDOWS) || defined(QT_NO_IMAGE_TRANSFORMATION))
#define QT_NO_STYLE_AQUA
#endif

// Vertical group boxes
#if !defined(QT_NO_VGROUPBOX) && (defined(QT_NO_HGROUPBOX))
#define QT_NO_VGROUPBOX
#endif

// Horizontal button groups
#if !defined(QT_NO_HBUTTONGROUP) && (defined(QT_NO_BUTTONGROUP))
#define QT_NO_HBUTTONGROUP
#endif

// Server to play sound
#if !defined(QT_NO_QWS_SOUNDSERVER) && (defined(QT_NO_SOUND) || defined(QT_NO_DIR) || defined(QT_NO_DNS))
#define QT_NO_QWS_SOUNDSERVER
#endif

// Hebrew Codec
#if !defined(QT_NO_CODEC_HEBREW) && (defined(QT_NO_CODECS) || defined(QT_NO_COMPLEXTEXT))
#define QT_NO_CODEC_HEBREW
#endif

// Dynamic module linking
#if !defined(QT_NO_COMPONENT) && (defined(QT_NO_QUUID_STRING) || defined(QT_NO_SETTINGS) || defined(QT_NO_SPRINTF) || defined(QT_NO_LIBRARY))
#define QT_NO_COMPONENT
#endif

// QHeader
#if !defined(QT_NO_HEADER) && (defined(QT_NO_STYLE) || defined(QT_NO_ICONSET))
#define QT_NO_HEADER
#endif

// Vertical button groups
#if !defined(QT_NO_VBUTTONGROUP) && (defined(QT_NO_HBUTTONGROUP))
#define QT_NO_VBUTTONGROUP
#endif

// Big Codecs (eg. CJK)
#if !defined(QT_NO_BIG_CODECS) && (defined(QT_NO_CODEC_HEBREW))
#define QT_NO_BIG_CODECS
#endif

// Tool-buttons
#if !defined(QT_NO_TOOLBUTTON) && (defined(QT_NO_BUTTON) || defined(QT_NO_ICONSET) || defined(QT_NO_STYLE))
#define QT_NO_TOOLBUTTON
#endif

// Document Object Model
#if !defined(QT_NO_DOM) && (defined(QT_NO_XML) || defined(QT_NO_MIME))
#define QT_NO_DOM
#endif

// Scrollable view widgets
#if !defined(QT_NO_SCROLLVIEW) && (defined(QT_NO_SCROLLBAR) || defined(QT_NO_FRAME))
#define QT_NO_SCROLLVIEW
#endif

// Tab-bars
#if !defined(QT_NO_TABBAR) && (defined(QT_NO_TOOLBUTTON))
#define QT_NO_TABBAR
#endif

// QTextCodecPlugin
#if !defined(QT_NO_TEXTCODECPLUGIN) && (defined(QT_NO_COMPONENT) || defined(QT_NO_TEXTCODEC))
#define QT_NO_TEXTCODECPLUGIN
#endif

// Drag and drop
#if !defined(QT_NO_DRAGANDDROP) && (defined(QT_NO_MIME) || defined(QT_NO_QWS_PROPERTIES) || defined(QT_NO_IMAGEIO_XPM))
#define QT_NO_DRAGANDDROP
#endif

// QGridView
#if !defined(QT_NO_GRIDVIEW) && (defined(QT_NO_SCROLLVIEW))
#define QT_NO_GRIDVIEW
#endif

// QImageFormatPlugin
#if !defined(QT_NO_IMAGEFORMATPLUGIN) && (defined(QT_NO_COMPONENT) || defined(QT_NO_IMAGEIO))
#define QT_NO_IMAGEFORMATPLUGIN
#endif

// Single-line edits
#if !defined(QT_NO_LINEEDIT) && (defined(QT_NO_FRAME) || defined(QT_NO_RICHTEXT))
#define QT_NO_LINEEDIT
#endif

// Popup-menus
#if !defined(QT_NO_POPUPMENU) && (defined(QT_NO_MENUDATA) || defined(QT_NO_FRAME))
#define QT_NO_POPUPMENU
#endif

// QCanvas
#if !defined(QT_NO_CANVAS) && (defined(QT_NO_SCROLLVIEW) || defined(QT_NO_BEZIER))
#define QT_NO_CANVAS
#endif

// QListBox
#if !defined(QT_NO_LISTBOX) && (defined(QT_NO_SCROLLVIEW) || defined(QT_NO_STRINGLIST))
#define QT_NO_LISTBOX
#endif

// Menu bars
#if !defined(QT_NO_MENUBAR) && (defined(QT_NO_POPUPMENU))
#define QT_NO_MENUBAR
#endif

// HTTP file access
#if !defined(QT_NO_NETWORKPROTOCOL_HTTP) && (defined(QT_NO_NETWORKPROTOCOL) || defined(QT_NO_DNS))
#define QT_NO_NETWORKPROTOCOL_HTTP
#endif

// RichText (HTML) tables and images
#if !defined(QT_NO_TEXTCUSTOMITEM) && (defined(QT_NO_RICHTEXT) || defined(QT_NO_MIME))
#define QT_NO_TEXTCUSTOMITEM
#endif

// QDateTimeEdit
#if !defined(QT_NO_DATETIMEEDIT) && (defined(QT_NO_RICHTEXT) || defined(QT_NO_SPINWIDGET) || defined(QT_NO_DATESTRING))
#define QT_NO_DATETIMEEDIT
#endif

// Push-buttons
#if !defined(QT_NO_PUSHBUTTON) && (defined(QT_NO_BUTTON) || defined(QT_NO_POPUPMENU))
#define QT_NO_PUSHBUTTON
#endif

// FTP file access
#if !defined(QT_NO_NETWORKPROTOCOL_FTP) && (defined(QT_NO_NETWORKPROTOCOL) || defined(QT_NO_DNS) || defined(QT_NO_TEXTDATE))
#define QT_NO_NETWORKPROTOCOL_FTP
#endif

// Scalable Vector Graphics (SVG)
#if !defined(QT_NO_SVG) && (defined(QT_NO_DOM) || defined(QT_NO_TRANSFORMATIONS) || defined(QT_NO_SPRINTF))
#define QT_NO_SVG
#endif

// QWidgetPlugin
#if !defined(QT_NO_WIDGETPLUGIN) && (defined(QT_NO_COMPONENT) || defined(QT_NO_ICONSET))
#define QT_NO_WIDGETPLUGIN
#endif

// QIconView
#if !defined(QT_NO_ICONVIEW) && (defined(QT_NO_SCROLLVIEW) || defined(QT_NO_IMAGEIO_XPM) || defined(QT_NO_IMAGE_HEURISTIC_MASK))
#define QT_NO_ICONVIEW
#endif

// Tab widgets
#if !defined(QT_NO_TABWIDGET) && (defined(QT_NO_TABBAR) || defined(QT_NO_WIDGETSTACK))
#define QT_NO_TABWIDGET
#endif

// "What's this" help
#if !defined(QT_NO_WHATSTHIS) && (defined(QT_NO_TOOLTIP) || defined(QT_NO_TOOLBUTTON))
#define QT_NO_WHATSTHIS
#endif

// Rich text edit
#if !defined(QT_NO_TEXTEDIT) && (defined(QT_NO_RICHTEXT) || defined(QT_NO_SCROLLVIEW))
#define QT_NO_TEXTEDIT
#endif

// Multi-line edits
#if !defined(QT_NO_MULTILINEEDIT) && (defined(QT_NO_TEXTEDIT))
#define QT_NO_MULTILINEEDIT
#endif

// QTextView
#if !defined(QT_NO_TEXTVIEW) && (defined(QT_NO_TEXTEDIT))
#define QT_NO_TEXTVIEW
#endif

// Spin boxes
#if !defined(QT_NO_SPINBOX) && (defined(QT_NO_RANGECONTROL) || defined(QT_NO_SPINWIDGET) || defined(QT_NO_LINEEDIT) || defined(QT_NO_VALIDATOR))
#define QT_NO_SPINBOX
#endif

// QMessageBox
#if !defined(QT_NO_MESSAGEBOX) && (defined(QT_NO_DIALOG) || defined(QT_NO_PUSHBUTTON) || defined(QT_NO_LABEL))
#define QT_NO_MESSAGEBOX
#endif

// SQL classes
#if !defined(QT_NO_SQL) && (defined(QT_NO_MESSAGEBOX))
#define QT_NO_SQL
#endif

// QTextBrowser
#if !defined(QT_NO_TEXTBROWSER) && (defined(QT_NO_TEXTVIEW) || defined(QT_NO_MIME))
#define QT_NO_TEXTBROWSER
#endif

// QProgressDialog
#if !defined(QT_NO_PROGRESSDIALOG) && (defined(QT_NO_SEMIMODAL) || defined(QT_NO_LABEL) || defined(QT_NO_PUSHBUTTON) || defined(QT_NO_PROGRESSBAR))
#define QT_NO_PROGRESSDIALOG
#endif

// QWizard
#if !defined(QT_NO_WIZARD) && (defined(QT_NO_DIALOG) || defined(QT_NO_WIDGETSTACK) || defined(QT_NO_PUSHBUTTON) || defined(QT_NO_LAYOUT) || defined(QT_NO_LABEL))
#define QT_NO_WIZARD
#endif

// QSqlForm
#if !defined(QT_NO_SQL_FORM) && (defined(QT_NO_SQL) || defined(QT_NO_PROPERTIES))
#define QT_NO_SQL_FORM
#endif

// QListView
#if !defined(QT_NO_LISTVIEW) && (defined(QT_NO_SCROLLVIEW) || defined(QT_NO_HEADER) || defined(QT_NO_LINEEDIT))
#define QT_NO_LISTVIEW
#endif

// QTabDialog
#if !defined(QT_NO_TABDIALOG) && (defined(QT_NO_DIALOG) || defined(QT_NO_PUSHBUTTON) || defined(QT_NO_LAYOUT) || defined(QT_NO_TABWIDGET))
#define QT_NO_TABDIALOG
#endif

// QComboBox
#if !defined(QT_NO_COMBOBOX) && (defined(QT_NO_LISTBOX) || defined(QT_NO_LINEEDIT) || defined(QT_NO_POPUPMENU))
#define QT_NO_COMBOBOX
#endif

// Main-windows
#if !defined(QT_NO_MAINWINDOW) && (defined(QT_NO_STRINGLIST) || defined(QT_NO_POPUPMENU) || defined(QT_NO_TITLEBAR) || defined(QT_NO_RESIZEHANDLER) || defined(QT_NO_TOOLBUTTON) || defined(QT_NO_STATUSBAR))
#define QT_NO_MAINWINDOW
#endif

// Toolbars
#if !defined(QT_NO_TOOLBAR) && (defined(QT_NO_MAINWINDOW))
#define QT_NO_TOOLBAR
#endif

// QColorDialog
#if !defined(QT_NO_COLORDIALOG) && (defined(QT_NO_DIALOG) || defined(QT_NO_LABEL) || defined(QT_NO_PUSHBUTTON) || defined(QT_NO_LINEEDIT) || defined(QT_NO_VALIDATOR) || defined(QT_NO_GRIDVIEW))
#define QT_NO_COLORDIALOG
#endif

// QErrorMessage
#if !defined(QT_NO_ERRORMESSAGE) && (defined(QT_NO_DIALOG) || defined(QT_NO_PUSHBUTTON) || defined(QT_NO_LABEL) || defined(QT_NO_CHECKBOX) || defined(QT_NO_TEXTVIEW))
#define QT_NO_ERRORMESSAGE
#endif

// QAction
#if !defined(QT_NO_ACTION) && (defined(QT_NO_TOOLBUTTON) || defined(QT_NO_COMBOBOX))
#define QT_NO_ACTION
#endif

// QWorkSpace
#if !defined(QT_NO_WORKSPACE) && (defined(QT_NO_SCROLLBAR) || defined(QT_NO_VBOX) || defined(QT_NO_TITLEBAR) || defined(QT_NO_RESIZEHANDLER) || defined(QT_NO_POPUPMENU) || defined(QT_NO_LABEL) || defined(QT_NO_TOOLBUTTON))
#define QT_NO_WORKSPACE
#endif

// QTable
#if !defined(QT_NO_TABLE) && (defined(QT_NO_COMBOBOX) || defined(QT_NO_HEADER) || defined(QT_NO_CHECKBOX))
#define QT_NO_TABLE
#endif

// QFontDialog
#if !defined(QT_NO_FONTDIALOG) && (defined(QT_NO_DIALOG) || defined(QT_NO_FONTDATABASE) || defined(QT_NO_COMBOBOX) || defined(QT_NO_LABEL) || defined(QT_NO_CHECKBOX) || defined(QT_NO_PUSHBUTTON) || defined(QT_NO_VGROUPBOX))
#define QT_NO_FONTDIALOG
#endif

// QInputDialog
#if !defined(QT_NO_INPUTDIALOG) && (defined(QT_NO_DIALOG) || defined(QT_NO_COMBOBOX) || defined(QT_NO_LABEL) || defined(QT_NO_PUSHBUTTON) || defined(QT_NO_SPINBOX) || defined(QT_NO_WIDGETSTACK))
#define QT_NO_INPUTDIALOG
#endif

// SQL value editor widgets
#if !defined(QT_NO_SQL_EDIT_WIDGETS) && (defined(QT_NO_SQL) || defined(QT_NO_SPINBOX) || defined(QT_NO_COMBOBOX) || defined(QT_NO_CHECKBOX) || defined(QT_NO_DATETIMEEDIT))
#define QT_NO_SQL_EDIT_WIDGETS
#endif

// QPrintDialog
#if !defined(QT_NO_PRINTDIALOG) && (defined(QT_NO_DIALOG) || defined(QT_NO_LISTVIEW) || defined(QT_NO_PRINTER) || defined(QT_NO_COMBOBOX) || defined(QT_NO_LABEL) || defined(QT_NO_BUTTONGROUP) || defined(QT_NO_SPINBOX) || defined(QT_NO_RADIOBUTTON) || defined(QT_NO_PUSHBUTTON))
#define QT_NO_PRINTDIALOG
#endif

// QFileDialog
#if !defined(QT_NO_FILEDIALOG) && (defined(QT_NO_MESSAGEBOX) || defined(QT_NO_LISTVIEW) || defined(QT_NO_NETWORKPROTOCOL) || defined(QT_NO_COMBOBOX) || defined(QT_NO_SEMIMODAL) || defined(QT_NO_REGEXP_CAPTURE) || defined(QT_NO_TOOLBUTTON) || defined(QT_NO_BUTTONGROUP) || defined(QT_NO_VBOX) || defined(QT_NO_SPLITTER) || defined(QT_NO_PROGRESSBAR) || defined(QT_NO_WIDGETSTACK) || defined(QT_NO_DATESTRING))
#define QT_NO_FILEDIALOG
#endif

// SQL table widgets
#if !defined(QT_NO_SQL_VIEW_WIDGETS) && (defined(QT_NO_SQL_FORM) || defined(QT_NO_SQL_EDIT_WIDGETS) || defined(QT_NO_TABLE))
#define QT_NO_SQL_VIEW_WIDGETS
#endif

