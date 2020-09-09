#include "EventManager.h"

#include "IOIMPL/LCFactory.h"

#include "EventDisplay.h"

ClassImp(hps::EventManager);

namespace hps {

    EventManager::EventManager(TEveManager* eve,
                               TGeoManager* geo,
                               EventDisplay* app,
                               std::vector<std::string> fileNames,
                               std::set<std::string> excludeColls,
                               double bY,
                               int verbose) :
            TEveEventManager("HPS Event Manager", ""),
            eve_(eve),
            geo_(geo),
            fileNames_(fileNames),
            reader_(nullptr),
            event_(new EventObjects(geo, excludeColls, bY, verbose)),
            app_(app),
            verbose_(verbose),
            excludeColls_(excludeColls) {
    }

    EventManager::~EventManager() {
        delete event_;
    }

    void EventManager::Open() {
        if (verbose_) {
            std::cout << "[ EventManager ] Opening reader... " << std::endl;
        }
        reader_ = IOIMPL::LCFactory::getInstance()->createLCReader(IO::LCReader::directAccess);
        reader_->open(this->fileNames_);
        //maxEvents_ = reader_->getNumberOfEvents();
        //if (verbose_) {
        //    std::cout << "[ EventManager ] Max events set to " << maxEvents_ << std::endl;
        //}
        auto runHeader = reader_->readNextRunHeader();
        if (runHeader != nullptr) {
            runNumber_ = runHeader->getRunNumber();
        } else if (runHeader == nullptr) {
            if (verbose_) {
                std::cout << "[ EventManager ] Setting run number from first event ..." << std::endl;
            }
            auto event = reader_->readNextEvent();
            runNumber_ = event->getRunNumber();
            reader_->close();
            reader_->open(this->fileNames_);
            if (verbose_) {
                std::cout << "[ EventManager ] Done setting run number from first event!" << std::endl;
            }
        }
        if (verbose_) {
            std::cout << "[ EventManager ] Run number set to: " << runNumber_ << std::endl;
            std::cout << "[ EventManager ] Done opening reader!" << std::endl;
        }
    }

    void EventManager::NextEvent() {
        GotoEvent(eventNum_ + 1);
    }

    void EventManager::loadEvent(EVENT::LCEvent* event) {
        eve_->GetCurrentEvent()->DestroyElements();
        if (verbose_) {
            std::cout << "[ EventManager ] Loading LCIO event: " << event->getEventNumber() << std::endl;
        }
        event_->build(eve_, event);
        if (verbose_) {
            std::cout << "[ EventManager ] Done loading event!" << std::endl;
        }
    }

    void EventManager::GotoEvent(Int_t i) {
        if (verbose_) {
            std::cout << "[ EventManager ] GotoEvent: " << i << std::endl;
        }
        /*
        if (i > maxEvents_ - 1) {
            std::cerr << "[ EventManager ] Event num " << i
                    << " is greater than max events " << maxEvents_
                    << " (command ignored) " << std::endl;
            return;
        } else*/
        if (i < 0) {
            std::cerr << "[ EventManager ] Event number is not valid: " << i << std::endl;
            return;
        } else if (i == eventNum_) {
            std::cerr << "[ EventManager ] Event is already loaded: " << i << std::endl;
            return;
        }
        EVENT::LCEvent* event = nullptr;
        if (i == (eventNum_ + 1)) {
            if (verbose_) {
                std::cout << "[ EventManager ] Reading next event" << std::endl;
            }
            try {
                event = reader_->readNextEvent();
            } catch (IO::IOException& ioe) {
                std::cerr << "[ EventManager ] [ ERROR ] " << ioe.what() << std::endl;
            } catch (std::exception& e) {
                std::cerr << "[ EventManager ] [ ERROR ] " << e.what() << std::endl;
            }
        } else {
            if (verbose_) {
                std::cout << "[ EventManager ] Seeking event " << i
                        << " with run number " << runNumber_ << std::endl;
            }
            try {
                event = reader_->readEvent(runNumber_, i);
                if (event == nullptr) {
                    std::cerr << "[ EventManager ] [ ERROR ] Seeking failed!" << std::endl;
                }
            } catch (IO::IOException& ioe) {
                std::cerr << "[ EventManager ] [ ERROR ] " << ioe.what() << std::endl;
            } catch (std::exception& e) {
                std::cerr << "[ EventManager ] [ ERROR ] " << e.what() << std::endl;
            }
        }
        if (event != nullptr) {
            loadEvent(event);
            eventNum_ = i;
        } else {
            std::cerr << "[ EventManager ] [ ERROR ] Failed to read next event!" << std::endl;
        }
        eve_->FullRedraw3D(false);
    }

    void EventManager::PrevEvent() {
        if (verbose_ > 1) {
            std::cout << "[ EventManager ] PrevEvent" << std::endl;
        }
        if (eventNum_ > 0) {
            GotoEvent(eventNum_ - 1);
        } else {
            std::cerr << "[ EventManager ] [ ERROR ] Already at first event!" << std::endl;
        }
    }

    void EventManager::SetEventNumber() {
        if (verbose_) {
            std::cout << "[ EventManager ] Set event number: " << app_->getCurrentEventNumber() << std::endl;
        }
        if (app_->getCurrentEventNumber() > -1) {
            GotoEvent(app_->getCurrentEventNumber());
        } else {
            std::cerr << "[ EventManager ] [ ERROR ] Event number is not valid: "
                    << app_->getCurrentEventNumber() << std::endl;

        }
    }

    /*
    void EventManager::Close() {
        std::cout << "[ EventManager ] : Close" << std::endl;
    }

    void EventManager::AfterNewEventLoaded() {
        std::cout << "[ EventManager ] : AfterNewEventLoaded" << std::endl;
    }

    void EventManager::AddNewEventCommand(const TString& cmd) {
        std::cout << "[ EventManager ] : AddNewEventCommand - " << cmd << std::endl;
    }

    void EventManager::RemoveNewEventCommand(const TString& cmd) {
        std::cout << "[ EventManager ] : RemoveNewEventCommand - " << cmd << std::endl;
    }

    void EventManager::ClearNewEventCommands() {
        std::cout << "[ EventManager ] : ClearNewEventCommands" << std::endl;
    }
    */
}
