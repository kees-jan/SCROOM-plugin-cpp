#pragma once

#include <scroom/logger.hh>
#include <scroom/plugininformationinterface.hh>
#include <scroom/presentationinterface.hh>
#include <scroom/utilities.hh>

class Sep : public PluginInformationInterface,
            public OpenInterface,
            virtual public Scroom::Utils::Base {
public:
  typedef std::shared_ptr<Sep> Ptr;

private:
  Sep();
  Scroom::Logger logger;

public:
  static Ptr create();

public:
  virtual ~Sep();

  ////////////////////////////////////////////////////////////////////////
  // PluginInformationInterface

  virtual std::string getPluginName();
  virtual std::string getPluginVersion();
  virtual void registerCapabilities(ScroomPluginInterface::Ptr host);

  ////////////////////////////////////////////////////////////////////////
  // OpenPresentationInterface

  virtual std::list<GtkFileFilter *> getFilters();
  virtual void open(const std::string &fileName,
                    ScroomInterface::Ptr const &scroomInterface);
};
