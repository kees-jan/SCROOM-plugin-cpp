#pragma once

#include <string>
#include <map>
#include <list>
#include <set>

#include <scroom/colormappable.hh>
#include <scroom/observable.hh>
#include <scroom/presentationinterface.hh>
#include <scroom/scroominterface.hh>
#include <scroom/tiledbitmapinterface.hh>
#include <scroom/transformpresentation.hh>

class SepPresentation : public SourcePresentation,
                         public virtual Scroom::Utils::Base
{
public:
  typedef boost::shared_ptr<SEPPresentation> Ptr;

private:
  typedef std::set<ViewInterface::WeakPtr> Views;

  std::string fileName;
  SEP* sep;
  int height;
  int width;
  TiledBitmapInterface::Ptr tbi;
  int bps;
  int spp;
  std::map<std::string, std::string> properties;
  Views views;
  ColormapHelper::Ptr colormapHelper;
  TransformationData::Ptr transformationData;

private:
  TiffPresentation();

public:
  virtual ~TiffPresentation();

  static Ptr create();

  /**
   * Called when this presentation should go away.
   *
   * Note that this doesn't happen automatically, since the
   * TiledBitmapInterface has a reference to this presentation, via
   * the SourcePresentation.
   */
  void destroy();

  bool load(const std::string& fileName);
  TransformationData::Ptr getTransformationData() const;

  ////////////////////////////////////////////////////////////////////////
  // PresentationInterface
  ////////////////////////////////////////////////////////////////////////

  virtual Scroom::Utils::Rectangle<double> getRect();
  virtual void redraw(ViewInterface::Ptr const& vi, cairo_t* cr, Scroom::Utils::Rectangle<double> presentationArea, int zoom);
  virtual bool getProperty(const std::string& name, std::string& value);
  virtual bool isPropertyDefined(const std::string& name);
  virtual std::string getTitle();

  ////////////////////////////////////////////////////////////////////////
  // PresentationBase
  ////////////////////////////////////////////////////////////////////////

  virtual void viewAdded(ViewInterface::WeakPtr viewInterface);
  virtual void viewRemoved(ViewInterface::WeakPtr vi);
  virtual std::set<ViewInterface::WeakPtr> getViews();

  ////////////////////////////////////////////////////////////////////////
  // SourcePresentation
  ////////////////////////////////////////////////////////////////////////
public:
  virtual void fillTiles(int startLine, int lineCount, int tileWidth, int firstTile, std::vector<Tile::Ptr>& tiles);
  virtual void done();

  ////////////////////////////////////////////////////////////////////////
  // Colormappable
  ////////////////////////////////////////////////////////////////////////

public:
  virtual void setColormap(Colormap::Ptr colormap);
  virtual Colormap::Ptr getOriginalColormap();
  virtual int getNumberOfColors();
  virtual Color getMonochromeColor();
  virtual void setMonochromeColor(const Color& c);
  virtual void setTransparentBackground();
  virtual void disableTransparentBackground();
  virtual bool getTransparentBackground();

private:
  void clearCaches();
};