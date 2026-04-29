// ESP32 stub for Trace - replaces Trace.cpp entirely
#include "Trace.hpp"
#include "Network.hpp"
#include "Peer.hpp"
#include "Path.hpp"
#include "CertificateOfMembership.hpp"
#include "CertificateOfOwnership.hpp"
#include "Capability.hpp"
#include "Tag.hpp"
#include "Revocation.hpp"

namespace ZeroTier {

void Trace::resettingPathsInScope(void* const tPtr, const Address& reporter, const InetAddress& reporterPhysicalAddress, const InetAddress& myPhysicalAddress, const InetAddress::IpScope scope) {}
void Trace::peerConfirmingUnknownPath(void* const tPtr, const uint64_t networkId, Peer& peer, const SharedPtr<Path>& path, const uint64_t packetId, const Packet::Verb verb) {}
void Trace::bondStateMessage(void* const tPtr, char* msg) {}
void Trace::peerLearnedNewPath(void* const tPtr, const uint64_t networkId, Peer& peer, const SharedPtr<Path>& newPath, const uint64_t packetId) {}
void Trace::peerRedirected(void* const tPtr, const uint64_t networkId, Peer& peer, const SharedPtr<Path>& newPath) {}
void Trace::incomingPacketMessageAuthenticationFailure(void* const tPtr, const SharedPtr<Path>& path, const uint64_t packetId, const Address& source, const unsigned int hops, const char* reason) {}
void Trace::incomingPacketInvalid(void* const tPtr, const SharedPtr<Path>& path, const uint64_t packetId, const Address& source, const unsigned int hops, const Packet::Verb verb, const char* reason) {}
void Trace::incomingPacketDroppedHELLO(void* const tPtr, const SharedPtr<Path>& path, const uint64_t packetId, const Address& source, const char* reason) {}
void Trace::outgoingNetworkFrameDropped(void* const tPtr, const SharedPtr<Network>& network, const MAC& sourceMac, const MAC& destMac, const unsigned int etherType, const unsigned int vlanId, const unsigned int frameLen, const char* reason) {}
void Trace::networkConfigRequestSent(void* const tPtr, const Network& network, const Address& controller) {}
void Trace::credentialRejected(void* const tPtr, const CertificateOfMembership& c, const char* reason) {}
void Trace::credentialRejected(void* const tPtr, const CertificateOfOwnership& c, const char* reason) {}
void Trace::credentialRejected(void* const tPtr, const Capability& c, const char* reason) {}
void Trace::credentialRejected(void* const tPtr, const Tag& c, const char* reason) {}
void Trace::credentialRejected(void* const tPtr, const Revocation& c, const char* reason) {}
void Trace::updateMemoizedSettings() {}
void Trace::_send(void* const tPtr, const Dictionary<ZT_MAX_REMOTE_TRACE_SIZE>& d, const Address& dest) {}
void Trace::_spamToAllNetworks(void* const tPtr, const Dictionary<ZT_MAX_REMOTE_TRACE_SIZE>& d, const Level level) {}

void Trace::networkFilter(
    void* const tPtr, const Network& network,
    const RuleResultLog& primaryRuleSetLog,
    const RuleResultLog* const matchingCapabilityRuleSetLog,
    const Capability* const matchingCapability,
    const Address& ztSource, const Address& ztDest,
    const MAC& macSource, const MAC& macDest,
    const uint8_t* const frameData,
    const unsigned int frameLen, const unsigned int etherType,
    const unsigned int vlanId, const bool noTee,
    const bool inbound, const int accept) {}

void Trace::incomingNetworkAccessDenied(
    void* const tPtr, const SharedPtr<Network>& network,
    const SharedPtr<Path>& path, const uint64_t packetId,
    const unsigned int packetLength, const Address& source,
    const Packet::Verb verb,
    bool credentialsRequested) {}

void Trace::incomingNetworkFrameDropped(
    void* const tPtr, const SharedPtr<Network>& network,
    const SharedPtr<Path>& path, const uint64_t packetId,
    const unsigned int packetLength, const Address& source,
    const Packet::Verb verb,
    const MAC& sourceMac, const MAC& destMac,
    const char* reason) {}

} // namespace ZeroTier
