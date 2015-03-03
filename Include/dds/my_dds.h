#pragma once

#include <nv_hhdds/nv_hhdds.h>
#include <include/includeAll.h>
#include <nv_hhdds/nv_types.h>
#include <vector>

using namespace std;

typedef unsigned int  ui; 
typedef unsigned char uc;

class dds
{
   public:
   bool dds_texture_load(const char* name);

   int getWidth(int surfaceId)
   {
	   if(surfaceId < 0 || surfaceId > (m_surface.size() - 1)) return -1;
	   return m_surface[surfaceId].m_width;
   }

   int getHeight(int surfaceId)
   {
	   if(surfaceId < 0 || surfaceId > (m_surface.size() - 1)) return -1;
	   return m_surface[surfaceId].m_height;
   }

   uc* getImageData(int surfaceId)
   {
	   if(surfaceId < 0 || surfaceId > (m_surface.size() - 1)) return 0;
	   return m_surface[surfaceId].m_image_data;
   }

   ui getImageSize(int surfaceId)
   {
	   if(surfaceId < 0 || surfaceId > (m_surface.size() - 1)) return -1;
	   return m_surface[surfaceId].m_image_size;
   }

   GLenum getFormat2() { return m_Format; }

   private:
   NVHHDDSImage* m_image_data;
   ui m_Faces, m_SurfacesPerFace;
   GLenum m_Format;

   NvU32 getPitch(NVHHDDSImage* img, NvU32 index);

   struct Surface
   {
	   ui m_width, m_height, m_depth, m_pitch;
	   uc* m_image_data;
	   ui  m_image_size;
   };

   vector<Surface> m_surface;
};
