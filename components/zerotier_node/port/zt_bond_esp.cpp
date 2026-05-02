// ESP32 stub for Bond - replaces Bond.cpp entirely
#include "Peer.hpp"
#include "Bond.hpp"

namespace ZeroTier {

// Static members
int Bond::_minReqMonitorInterval = 100;
uint8_t Bond::_defaultPolicy = 0;
std::string Bond::_defaultPolicyStr;
std::map<std::string, SharedPtr<Bond>> Bond::_bondPolicyTemplates;

// Constructors
Bond::Bond(const RuntimeEnvironment *renv) {}
Bond::Bond(const RuntimeEnvironment *renv, int policy, const SharedPtr<Peer>& peer) {}
Bond::Bond(const RuntimeEnvironment *renv, std::string& basePolicy, std::string& policyAlias, const SharedPtr<Peer>& peer) {}
Bond::Bond(const RuntimeEnvironment *renv, SharedPtr<Bond> originalBond, const SharedPtr<Peer>& peer) {}

// Static methods
SharedPtr<Bond> Bond::createBond(const RuntimeEnvironment* renv, const SharedPtr<Peer>& peer) { return SharedPtr<Bond>(); }
void Bond::destroyBond(uint64_t peerId) {}
void Bond::processBackgroundTasks(void* tPtr, int64_t now) {}

// Instance methods
void Bond::nominatePathToBond(const SharedPtr<Path>& path, int64_t now) {}
void Bond::recordOutgoingPacket(const SharedPtr<Path>& path, uint64_t packetId, uint16_t payloadLength, Packet::Verb verb, int32_t flowId, int64_t now) {}
void Bond::recordIncomingPacket(const SharedPtr<Path>& path, uint64_t packetId, uint16_t payloadLength, Packet::Verb verb, int32_t flowId, int64_t now) {}
void Bond::recordIncomingInvalidPacket(const SharedPtr<Path>& path) {}
void Bond::receivedQoS(const SharedPtr<Path>& path, int64_t now, int count, uint64_t* rx_id, uint16_t* rx_ts) {}
void Bond::processIncomingPathNegotiationRequest(uint64_t now, SharedPtr<Path>& path, int16_t remoteUtility) {}
SharedPtr<Path> Bond::getAppropriatePath(int64_t now, int32_t flowId) { return SharedPtr<Path>(); }

} // namespace ZeroTier
