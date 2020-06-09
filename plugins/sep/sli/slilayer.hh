#pragma once

#include <scroom/scroominterface.hh>

// Keep it simple for now and hardcode the allowed parameters
#define BPS 8
#define SPP 4

class SliLayer : public virtual Scroom::Utils::Base
{
public:
  typedef boost::shared_ptr<SliLayer> Ptr;

  /** Height of the layer (in pixels) */
  int height;

  /** Width of the layer (in pixels) */
  int width;
  
  /** Samples per pixel */
  unsigned int spp;

  /** Bits per sample */
  unsigned int bps;

  // TODO decide whether these values should belong to SliLayer or SliPresentation
  /** The unit of measurement for XResolution and YResolution.
   * 
   *  The specification defines these values:
   *  1 = No absolute unit of measurement. Used for images that may have a non-square aspect * ratio, but no meaningful absolute dimensions
   *  2 = Inch
   *  3 = Centimeter
   */
  unsigned int ResolutionUnit;

  /** The number of pixels per ResolutionUnit in the ImageWidth direction */
  float Xresolution;

  /** The number of pixels per ResolutionUnit in the ImageLength direction */
  float Yresolution;

  /** Horizontal offset from the top-left point of the canvas (in pixels) */
  int xoffset;

  /** Vertical offset from the top-left point of the canvas (in pixels) */
  int yoffset;

  /** Filename without extension; used as an ID */
  std::string name;

  /** Absolute filepath to the tiff/sep file */
  std::string filepath;

  /** The memory chunk containing the bitmap */
  uint8_t* bitmap;

  /** Whether or not the layer is toggled to be visible */
  bool visible;

private:
  SliLayer();
  virtual bool load(const std::string &filepath);

public:
  /** Constructor */
  static Ptr create(const std::string &filepath, const std::string &name, int xoffset, int yoffset);

  /** Destructor */
  virtual ~SliLayer();

};