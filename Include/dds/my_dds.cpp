#include <dds/my_dds.h>
#include <file/my_file.h>

//------------------------------------------------------------------------------
//
NvU32 dds::getPitch(NVHHDDSImage* img,NvU32 index)
{
  if (!img->compressed)
    return img->bytesPerPixel*img->mipwidth[index];

  switch (img->format)
  {
  case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
    return (img->mipwidth[index]*3)/6;
  case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
  case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
    return img->mipwidth[index];
  }

  return 0;
}


//------------------------------------------------------------------------------
//
bool dds::dds_texture_load(const char* name)
{
  if (!name)
    return false;

  NVHHDDSImage* img = NVHHDDSLoad(name, 1);

  if (NULL == img)
    return false;

  m_Faces           = (img->cubemap ? 6 : 1);
  m_SurfacesPerFace = img->numMipmaps;
  m_Format          = img->format; // getFormat(img);

  for (NvU32 n = 0;n < m_Faces;n++)
  {
    for (NvU32 m = 0;m < m_SurfacesPerFace;m++)
    {
      Surface surface;
      const NvU32 surfaceId = m + n*m_SurfacesPerFace;

      surface.m_width  = img->mipwidth[surfaceId];
      surface.m_height = img->mipheight[surfaceId];
      surface.m_depth  = 1;
      surface.m_pitch  = getPitch(img,surfaceId);

	  surface.m_image_size = img->size[surfaceId];
	  surface.m_image_data = new uc[img->size[surfaceId]];
      memcpy(surface.m_image_data, img->data[surfaceId], img->size[surfaceId]);

      m_surface.push_back(surface); 
    }
  }

  NVHHDDSFree(img);
  
  return true;
}
