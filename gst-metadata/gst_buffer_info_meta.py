from copy import deepcopy

import gi
gi.require_version('Gst', '1.0')
from gi.repository import Gst, GObject

import sys
from ctypes import *


# Metadata structure that describes GstBufferInfo (C)
class GstBufferInfo(Structure):
    _fields_ = [("description", c_char_p)]

# Pointer to GstBufferInfo structure
GstBufferInfoPtr = POINTER(GstBufferInfo)

# Load C-lib
clib = CDLL("build/libgst_buffer_info_meta.so")

# Map ctypes arguments to C-style arguments
clib.gst_buffer_add_buffer_info_meta.argtypes = [c_void_p, GstBufferInfoPtr]
clib.gst_buffer_add_buffer_info_meta.restype = c_void_p

clib.gst_buffer_get_buffer_info_meta.argtypes = [c_void_p]
clib.gst_buffer_get_buffer_info_meta.restype = GstBufferInfoPtr

clib.gst_buffer_remove_buffer_info_meta.argtypes = [c_void_p]
clib.gst_buffer_remove_buffer_info_meta.restype = c_bool


def write_meta(buffer, description):
    """
        Writes GstBufferInfo as metadata to Gst.Buffer

        :param name: buffer
        :type name: Gst.Buffer

        :param name: custom information to be written
        :type name: str
    """  
    meta = GstBufferInfo()
    meta.description = description.encode("utf-8")
    clib.gst_buffer_add_buffer_info_meta(hash(buffer), meta)


def get_meta(buffer):
    """
        Get GstBufferInfo from Gst.Buffer

        :param name: buffer
        :type name: Gst.Buffer

        :rtype: GstBufferInfo
    """  
    res = clib.gst_buffer_get_buffer_info_meta(hash(buffer))
    return res.contents


def remove_meta(buffer):
    """
        Removes GstBufferInfo from Gst.Buffer

        :param name: buffer
        :type name: Gst.Buffer

        :rtype: bool
    """  
    return clib.gst_buffer_remove_buffer_info_meta(hash(buffer))
