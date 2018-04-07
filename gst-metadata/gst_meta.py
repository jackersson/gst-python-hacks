from copy import deepcopy

import gi
gi.require_version('Gst', '1.0')
from gi.repository import Gst, GObject

import sys
from ctypes import *


class GstBufferInfo(Structure):
    _fields_ = [("offset", c_int),   
                ("timestamp", c_double),
                ("data", c_char_p)]

GstBufferInfoPtr = POINTER(GstBufferInfo)

# TODO to parameter *.so filename
clib = CDLL("3rd_party/gstreamer/build/libgst_buffer_info_meta.so")

clib.gst_buffer_add_buffer_info_meta.argtypes = [c_void_p, GstBufferInfoPtr]
clib.gst_buffer_add_buffer_info_meta.restype = c_void_p

clib.gst_buffer_get_buffer_info_meta.argtypes = [c_void_p]
clib.gst_buffer_get_buffer_info_meta.restype = GstBufferInfoPtr


def gst_buffer_write_buffer_info_meta(buffer, offset, timestamp):
    meta = GstBufferInfo()
    meta.offset = offset
    meta.timestamp = timestamp
    clib.gst_buffer_add_buffer_info_meta(hash(buffer), meta)


def gst_buffer_get_buffer_info_meta(buffer):
    res = clib.gst_buffer_get_buffer_info_meta(hash(buffer))
    return res.contents
