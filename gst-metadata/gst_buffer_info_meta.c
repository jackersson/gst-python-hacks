 
#include "gst_buffer_info_meta.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

static gboolean gst_buffer_info_meta_init(GstMeta *meta, gpointer params, GstBuffer *buffer);
static gboolean gst_buffer_info_meta_transform(GstBuffer *transbuf, GstMeta *meta, GstBuffer *buffer,
                                               GQuark type, gpointer data);


GstBufferInfo* empty(){
    static GstBufferInfo info;
    info.description = "";
    return &info;
}
     

// Register metadata type and returns Gtype
// https://gstreamer.freedesktop.org/data/doc/gstreamer/head/gstreamer/html/gstreamer-GstMeta.html#gst-meta-api-type-register
GType gst_buffer_info_meta_api_get_type(void)
{
    static const gchar *tags[] = {NULL};
    static volatile GType type;
    if (g_once_init_enter (&type)) {
        GType _type = gst_meta_api_type_register("GstBufferInfoMetaAPI", tags);
        g_once_init_leave(&type, _type);
    }
    return type;
}

// GstMetaInfo provides info for specific metadata implementation
// https://gstreamer.freedesktop.org/data/doc/gstreamer/head/gstreamer/html/gstreamer-GstMeta.html#GstMetaInfo

const GstMetaInfo *gst_buffer_info_meta_get_info(void)
{
    static const GstMetaInfo *gst_buffer_info_meta_info = NULL;
 
    if (g_once_init_enter (&gst_buffer_info_meta_info)) {
        // Explanation of fields
        // https://gstreamer.freedesktop.org/documentation/design/meta.html#gstmeta1
        const GstMetaInfo *meta = gst_meta_register (GST_BUFFER_INFO_META_API_TYPE, /* api type */
                                                     "GstBufferInfoMeta",           /* implementation type */
                                                     sizeof (GstBufferInfoMeta),    /* size of the structure */
                                                     gst_buffer_info_meta_init,
                                                     (GstMetaFreeFunction) NULL,
                                                     gst_buffer_info_meta_transform);
        g_once_init_leave (&gst_buffer_info_meta_info, meta);
    }
    return gst_buffer_info_meta_info;
}
 
// Meta init function
// 4-th field in GstMetaInfo
static gboolean gst_buffer_info_meta_init(GstMeta *meta, gpointer params, GstBuffer *buffer)
{
    GstBufferInfoMeta *gst_buffer_info_meta = (GstBufferInfoMeta*)meta;     
    gst_buffer_info_meta->info.offset = 0;
    gst_buffer_info_meta->info.timestamp = 0;
    return TRUE;
}
 
// Meta transform function
// 5-th field in GstMetaInfo
static gboolean gst_buffer_info_meta_transform(GstBuffer *transbuf, GstMeta *meta, GstBuffer *buffer,
                                               GQuark type, gpointer data)
{
    GstBufferInfoMeta *gst_buffer_info_meta = (GstBufferInfoMeta *)meta;
    gst_buffer_add_buffer_info_meta(transbuf, &(gst_buffer_info_meta->info));
    return TRUE;
}

// Only for Python : return GstAnnotationArray instead of GstAnnotationsMeta
// Special for Python
GstBufferInfo* gst_buffer_get_buffer_info_meta(GstBuffer* buffer)
{   
    GstBufferInfoMeta* meta = (GstBufferInfoMeta*)gst_buffer_get_meta((buffer), GST_BUFFER_INFO_META_API_TYPE);
    
    if (meta == NULL)
        return empty();
    else
        return &meta->info;   
}

 
GstBufferInfoMeta* gst_buffer_add_buffer_info_meta( GstBuffer *buffer
                                                  , GstBufferInfo* buffer_info )
{   
    GstBufferInfoMeta *gst_buffer_info_meta = NULL;

    g_return_val_if_fail(GST_IS_BUFFER(buffer), NULL);

    if ( ! gst_buffer_is_writable(buffer))
        return gst_buffer_info_meta;

    gst_buffer_info_meta = (GstBufferInfoMeta *) gst_buffer_add_meta (buffer, GST_BUFFER_INFO_META_INFO, NULL);

    if (buffer_info->data != NULL)
    {
        gst_buffer_info_meta->info.description = malloc(strlen(buffer_info->description) + 1);
        strcpy(gst_buffer_info_meta->info.description, buffer_info->description);
    }

    return gst_buffer_info_meta;
}

