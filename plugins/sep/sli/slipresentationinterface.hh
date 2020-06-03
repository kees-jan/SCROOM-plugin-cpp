#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include "slilayer.hh"


class SliPresentationInterface
{
public:
  typedef boost::shared_ptr<SliPresentationInterface> Ptr;
  typedef boost::weak_ptr<SliPresentationInterface> WeakPtr;

  virtual ~SliPresentationInterface() {}

  virtual void toggleLayer(int index)=0;

  virtual std::vector<SliLayer::Ptr>& getLayers()=0;

};