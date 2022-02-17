#include <time.h>

#include "buffer.h"
#include "ipfix.h"
#include "logging.h"
#include "net.h"
#include "str.h"


ipfix::ipfix()
{
	field_types.insert({ 0, { "Reserved", dt_reserved } });
	field_types.insert({ 1, { "octetDeltaCount", dt_unsigned64 } });
	field_types.insert({ 2, { "packetDeltaCount", dt_unsigned64 } });
	field_types.insert({ 3, { "deltaFlowCount", dt_unsigned64 } });
	field_types.insert({ 4, { "protocolIdentifier", dt_unsigned8 } });
	field_types.insert({ 5, { "ipClassOfService", dt_unsigned8 } });
	field_types.insert({ 6, { "tcpControlBits", dt_unsigned16 } });
	field_types.insert({ 7, { "sourceTransportPort", dt_unsigned16 } });
	field_types.insert({ 8, { "sourceIPv4Address", dt_ipv4Address } });
	field_types.insert({ 9, { "sourceIPv4PrefixLength", dt_unsigned8 } });
	field_types.insert({ 10, { "ingressInterface", dt_unsigned32 } });
	field_types.insert({ 11, { "destinationTransportPort", dt_unsigned16 } });
	field_types.insert({ 12, { "destinationIPv4Address", dt_ipv4Address } });
	field_types.insert({ 13, { "destinationIPv4PrefixLength", dt_unsigned8 } });
	field_types.insert({ 14, { "egressInterface", dt_unsigned32 } });
	field_types.insert({ 15, { "ipNextHopIPv4Address", dt_ipv4Address } });
	field_types.insert({ 16, { "bgpSourceAsNumber", dt_unsigned32 } });
	field_types.insert({ 17, { "bgpDestinationAsNumber", dt_unsigned32 } });
	field_types.insert({ 18, { "bgpNextHopIPv4Address", dt_ipv4Address } });
	field_types.insert({ 19, { "postMCastPacketDeltaCount", dt_unsigned64 } });
	field_types.insert({ 20, { "postMCastOctetDeltaCount", dt_unsigned64 } });
	field_types.insert({ 21, { "flowEndSysUpTime", dt_unsigned32 } });
	field_types.insert({ 22, { "flowStartSysUpTime", dt_unsigned32 } });
	field_types.insert({ 23, { "postOctetDeltaCount", dt_unsigned64 } });
	field_types.insert({ 24, { "postPacketDeltaCount", dt_unsigned64 } });
	field_types.insert({ 25, { "minimumIpTotalLength", dt_unsigned64 } });
	field_types.insert({ 26, { "maximumIpTotalLength", dt_unsigned64 } });
	field_types.insert({ 27, { "sourceIPv6Address", dt_ipv6Address } });
	field_types.insert({ 28, { "destinationIPv6Address", dt_ipv6Address } });
	field_types.insert({ 29, { "sourceIPv6PrefixLength", dt_unsigned8 } });
	field_types.insert({ 30, { "destinationIPv6PrefixLength", dt_unsigned8 } });
	field_types.insert({ 31, { "flowLabelIPv6", dt_unsigned32 } });
	field_types.insert({ 32, { "icmpTypeCodeIPv4", dt_unsigned16 } });
	field_types.insert({ 33, { "igmpType", dt_unsigned8 } });
	field_types.insert({ 34, { "samplingInterval", dt_unsigned32 } });
	field_types.insert({ 35, { "samplingAlgorithm", dt_unsigned8 } });
	field_types.insert({ 36, { "flowActiveTimeout", dt_unsigned16 } });
	field_types.insert({ 37, { "flowIdleTimeout", dt_unsigned16 } });
	field_types.insert({ 38, { "engineType", dt_unsigned8 } });
	field_types.insert({ 39, { "engineId", dt_unsigned8 } });
	field_types.insert({ 40, { "exportedOctetTotalCount", dt_unsigned64 } });
	field_types.insert({ 41, { "exportedMessageTotalCount", dt_unsigned64 } });
	field_types.insert({ 42, { "exportedFlowRecordTotalCount", dt_unsigned64 } });
	field_types.insert({ 43, { "ipv4RouterSc", dt_ipv4Address } });
	field_types.insert({ 44, { "sourceIPv4Prefix", dt_ipv4Address } });
	field_types.insert({ 45, { "destinationIPv4Prefix", dt_ipv4Address } });
	field_types.insert({ 46, { "mplsTopLabelType", dt_unsigned8 } });
	field_types.insert({ 47, { "mplsTopLabelIPv4Address", dt_ipv4Address } });
	field_types.insert({ 48, { "samplerId", dt_unsigned8 } });
	field_types.insert({ 49, { "samplerMode", dt_unsigned8 } });
	field_types.insert({ 50, { "samplerRandomInterval", dt_unsigned32 } });
	field_types.insert({ 51, { "classId", dt_unsigned8 } });
	field_types.insert({ 52, { "minimumTTL", dt_unsigned8 } });
	field_types.insert({ 53, { "maximumTTL", dt_unsigned8 } });
	field_types.insert({ 54, { "fragmentIdentification", dt_unsigned32 } });
	field_types.insert({ 55, { "postIpClassOfService", dt_unsigned8 } });
	field_types.insert({ 56, { "sourceMacAddress", dt_macAddress } });
	field_types.insert({ 57, { "postDestinationMacAddress", dt_macAddress } });
	field_types.insert({ 58, { "vlanId", dt_unsigned16 } });
	field_types.insert({ 59, { "postVlanId", dt_unsigned16 } });
	field_types.insert({ 60, { "ipVersion", dt_unsigned8 } });
	field_types.insert({ 61, { "flowDirection", dt_unsigned8 } });
	field_types.insert({ 62, { "ipNextHopIPv6Address", dt_ipv6Address } });
	field_types.insert({ 63, { "bgpNextHopIPv6Address", dt_ipv6Address } });
	field_types.insert({ 64, { "ipv6ExtensionHeaders", dt_unsigned32 } });
	field_types.insert({ 70, { "mplsTopLabelStackSection", dt_octetArray } });
	field_types.insert({ 71, { "mplsLabelStackSection2", dt_octetArray } });
	field_types.insert({ 72, { "mplsLabelStackSection3", dt_octetArray } });
	field_types.insert({ 73, { "mplsLabelStackSection4", dt_octetArray } });
	field_types.insert({ 74, { "mplsLabelStackSection5", dt_octetArray } });
	field_types.insert({ 75, { "mplsLabelStackSection6", dt_octetArray } });
	field_types.insert({ 76, { "mplsLabelStackSection7", dt_octetArray } });
	field_types.insert({ 77, { "mplsLabelStackSection8", dt_octetArray } });
	field_types.insert({ 78, { "mplsLabelStackSection9", dt_octetArray } });
	field_types.insert({ 79, { "mplsLabelStackSection10", dt_octetArray } });
	field_types.insert({ 80, { "destinationMacAddress", dt_macAddress } });
	field_types.insert({ 81, { "postSourceMacAddress", dt_macAddress } });
	field_types.insert({ 82, { "interfaceName", dt_string } });
	field_types.insert({ 83, { "interfaceDescription", dt_string } });
	field_types.insert({ 84, { "samplerName", dt_string } });
	field_types.insert({ 85, { "octetTotalCount", dt_unsigned64 } });
	field_types.insert({ 86, { "packetTotalCount", dt_unsigned64 } });
	field_types.insert({ 87, { "flagsAndSamplerId", dt_unsigned32 } });
	field_types.insert({ 88, { "fragmentOffset", dt_unsigned16 } });
	field_types.insert({ 89, { "forwardingStatus", dt_unsigned8 } });
	field_types.insert({ 90, { "mplsVpnRouteDistinguisher", dt_octetArray } });
	field_types.insert({ 91, { "mplsTopLabelPrefixLength", dt_unsigned8 } });
	field_types.insert({ 92, { "srcTrafficIndex", dt_unsigned32 } });
	field_types.insert({ 93, { "dstTrafficIndex", dt_unsigned32 } });
	field_types.insert({ 94, { "applicationDescription", dt_string } });
	field_types.insert({ 95, { "applicationId", dt_octetArray } });
	field_types.insert({ 96, { "applicationName", dt_string } });
	field_types.insert({ 97, { "Assigned for NetFlow v9 compatibility", dt_reserved } });
	field_types.insert({ 98, { "postIpDiffServCodePoint", dt_unsigned8 } });
	field_types.insert({ 99, { "multicastReplicationFactor", dt_unsigned32 } });
	field_types.insert({ 100, { "className", dt_string } });
	field_types.insert({ 101, { "classificationEngineId", dt_unsigned8 } });
	field_types.insert({ 102, { "layer2packetSectionOffset", dt_unsigned16 } });
	field_types.insert({ 103, { "layer2packetSectionSize", dt_unsigned16 } });
	field_types.insert({ 104, { "layer2packetSectionData", dt_octetArray } });
	field_types.insert({ 128, { "bgpNextAdjacentAsNumber", dt_unsigned32 } });
	field_types.insert({ 129, { "bgpPrevAdjacentAsNumber", dt_unsigned32 } });
	field_types.insert({ 130, { "exporterIPv4Address", dt_ipv4Address } });
	field_types.insert({ 131, { "exporterIPv6Address", dt_ipv6Address } });
	field_types.insert({ 132, { "droppedOctetDeltaCount", dt_unsigned64 } });
	field_types.insert({ 133, { "droppedPacketDeltaCount", dt_unsigned64 } });
	field_types.insert({ 134, { "droppedOctetTotalCount", dt_unsigned64 } });
	field_types.insert({ 135, { "droppedPacketTotalCount", dt_unsigned64 } });
	field_types.insert({ 136, { "flowEndReason", dt_unsigned8 } });
	field_types.insert({ 137, { "commonPropertiesId", dt_unsigned64 } });
	field_types.insert({ 138, { "observationPointId", dt_unsigned64 } });
	field_types.insert({ 139, { "icmpTypeCodeIPv6", dt_unsigned16 } });
	field_types.insert({ 140, { "mplsTopLabelIPv6Address", dt_ipv6Address } });
	field_types.insert({ 141, { "lineCardId", dt_unsigned32 } });
	field_types.insert({ 142, { "portId", dt_unsigned32 } });
	field_types.insert({ 143, { "meteringProcessId", dt_unsigned32 } });
	field_types.insert({ 144, { "exportingProcessId", dt_unsigned32 } });
	field_types.insert({ 145, { "templateId", dt_unsigned16 } });
	field_types.insert({ 146, { "wlanChannelId", dt_unsigned8 } });
	field_types.insert({ 147, { "wlanSSID", dt_string } });
	field_types.insert({ 148, { "flowId", dt_unsigned64 } });
	field_types.insert({ 149, { "observationDomainId", dt_unsigned32 } });
	field_types.insert({ 150, { "flowStartSeconds", dt_dateTimeSeconds } });
	field_types.insert({ 151, { "flowEndSeconds", dt_dateTimeSeconds } });
	field_types.insert({ 152, { "flowStartMilliseconds", dt_dateTimeMilliseconds } });
	field_types.insert({ 153, { "flowEndMilliseconds", dt_dateTimeMilliseconds } });
	field_types.insert({ 154, { "flowStartMicroseconds", dt_dateTimeMicroseconds } });
	field_types.insert({ 155, { "flowEndMicroseconds", dt_dateTimeMicroseconds } });
	field_types.insert({ 156, { "flowStartNanoseconds", dt_dateTimeNanoseconds } });
	field_types.insert({ 157, { "flowEndNanoseconds", dt_dateTimeNanoseconds } });
	field_types.insert({ 158, { "flowStartDeltaMicroseconds", dt_unsigned32 } });
	field_types.insert({ 159, { "flowEndDeltaMicroseconds", dt_unsigned32 } });
	field_types.insert({ 160, { "systemInitTimeMilliseconds", dt_dateTimeMilliseconds } });
	field_types.insert({ 161, { "flowDurationMilliseconds", dt_unsigned32 } });
	field_types.insert({ 162, { "flowDurationMicroseconds", dt_unsigned32 } });
	field_types.insert({ 163, { "observedFlowTotalCount", dt_unsigned64 } });
	field_types.insert({ 164, { "ignoredPacketTotalCount", dt_unsigned64 } });
	field_types.insert({ 165, { "ignoredOctetTotalCount", dt_unsigned64 } });
	field_types.insert({ 166, { "notSentFlowTotalCount", dt_unsigned64 } });
	field_types.insert({ 167, { "notSentPacketTotalCount", dt_unsigned64 } });
	field_types.insert({ 168, { "notSentOctetTotalCount", dt_unsigned64 } });
	field_types.insert({ 169, { "destinationIPv6Prefix", dt_ipv6Address } });
	field_types.insert({ 170, { "sourceIPv6Prefix", dt_ipv6Address } });
	field_types.insert({ 171, { "postOctetTotalCount", dt_unsigned64 } });
	field_types.insert({ 172, { "postPacketTotalCount", dt_unsigned64 } });
	field_types.insert({ 173, { "flowKeyIndicator", dt_unsigned64 } });
	field_types.insert({ 174, { "postMCastPacketTotalCount", dt_unsigned64 } });
	field_types.insert({ 175, { "postMCastOctetTotalCount", dt_unsigned64 } });
	field_types.insert({ 176, { "icmpTypeIPv4", dt_unsigned8 } });
	field_types.insert({ 177, { "icmpCodeIPv4", dt_unsigned8 } });
	field_types.insert({ 178, { "icmpTypeIPv6", dt_unsigned8 } });
	field_types.insert({ 179, { "icmpCodeIPv6", dt_unsigned8 } });
	field_types.insert({ 180, { "udpSourcePort", dt_unsigned16 } });
	field_types.insert({ 181, { "udpDestinationPort", dt_unsigned16 } });
	field_types.insert({ 182, { "tcpSourcePort", dt_unsigned16 } });
	field_types.insert({ 183, { "tcpDestinationPort", dt_unsigned16 } });
	field_types.insert({ 184, { "tcpSequenceNumber", dt_unsigned32 } });
	field_types.insert({ 185, { "tcpAcknowledgementNumber", dt_unsigned32 } });
	field_types.insert({ 186, { "tcpWindowSize", dt_unsigned16 } });
	field_types.insert({ 187, { "tcpUrgentPointer", dt_unsigned16 } });
	field_types.insert({ 188, { "tcpHeaderLength", dt_unsigned8 } });
	field_types.insert({ 189, { "ipHeaderLength", dt_unsigned8 } });
	field_types.insert({ 190, { "totalLengthIPv4", dt_unsigned16 } });
	field_types.insert({ 191, { "payloadLengthIPv6", dt_unsigned16 } });
	field_types.insert({ 192, { "ipTTL", dt_unsigned8 } });
	field_types.insert({ 193, { "nextHeaderIPv6", dt_unsigned8 } });
	field_types.insert({ 194, { "mplsPayloadLength", dt_unsigned32 } });
	field_types.insert({ 195, { "ipDiffServCodePoint", dt_unsigned8 } });
	field_types.insert({ 196, { "ipPrecedence", dt_unsigned8 } });
	field_types.insert({ 197, { "fragmentFlags", dt_unsigned8 } });
	field_types.insert({ 198, { "octetDeltaSumOfSquares", dt_unsigned64 } });
	field_types.insert({ 199, { "octetTotalSumOfSquares", dt_unsigned64 } });
	field_types.insert({ 200, { "mplsTopLabelTTL", dt_unsigned8 } });
	field_types.insert({ 201, { "mplsLabelStackLength", dt_unsigned32 } });
	field_types.insert({ 202, { "mplsLabelStackDepth", dt_unsigned32 } });
	field_types.insert({ 203, { "mplsTopLabelExp", dt_unsigned8 } });
	field_types.insert({ 204, { "ipPayloadLength", dt_unsigned32 } });
	field_types.insert({ 205, { "udpMessageLength", dt_unsigned16 } });
	field_types.insert({ 206, { "isMulticast", dt_unsigned8 } });
	field_types.insert({ 207, { "ipv4IHL", dt_unsigned8 } });
	field_types.insert({ 208, { "ipv4Options", dt_unsigned32 } });
	field_types.insert({ 209, { "tcpOptions", dt_unsigned64 } });
	field_types.insert({ 210, { "paddingOctets", dt_octetArray } });
	field_types.insert({ 211, { "collectorIPv4Address", dt_ipv4Address } });
	field_types.insert({ 212, { "collectorIPv6Address", dt_ipv6Address } });
	field_types.insert({ 213, { "exportInterface", dt_unsigned32 } });
	field_types.insert({ 214, { "exportProtocolVersion", dt_unsigned8 } });
	field_types.insert({ 215, { "exportTransportProtocol", dt_unsigned8 } });
	field_types.insert({ 216, { "collectorTransportPort", dt_unsigned16 } });
	field_types.insert({ 217, { "exporterTransportPort", dt_unsigned16 } });
	field_types.insert({ 218, { "tcpSynTotalCount", dt_unsigned64 } });
	field_types.insert({ 219, { "tcpFinTotalCount", dt_unsigned64 } });
	field_types.insert({ 220, { "tcpRstTotalCount", dt_unsigned64 } });
	field_types.insert({ 221, { "tcpPshTotalCount", dt_unsigned64 } });
	field_types.insert({ 222, { "tcpAckTotalCount", dt_unsigned64 } });
	field_types.insert({ 223, { "tcpUrgTotalCount", dt_unsigned64 } });
	field_types.insert({ 224, { "ipTotalLength", dt_unsigned64 } });
	field_types.insert({ 225, { "postNATSourceIPv4Address", dt_ipv4Address } });
	field_types.insert({ 226, { "postNATDestinationIPv4Address", dt_ipv4Address } });
	field_types.insert({ 227, { "postNAPTSourceTransportPort", dt_unsigned16 } });
	field_types.insert({ 228, { "postNAPTDestinationTransportPort", dt_unsigned16 } });
	field_types.insert({ 229, { "natOriginatingAddressRealm", dt_unsigned8 } });
	field_types.insert({ 230, { "natEvent", dt_unsigned8 } });
	field_types.insert({ 231, { "initiatorOctets", dt_unsigned64 } });
	field_types.insert({ 232, { "responderOctets", dt_unsigned64 } });
	field_types.insert({ 233, { "firewallEvent", dt_unsigned8 } });
	field_types.insert({ 234, { "ingressVRFID", dt_unsigned32 } });
	field_types.insert({ 235, { "egressVRFID", dt_unsigned32 } });
	field_types.insert({ 236, { "VRFname", dt_string } });
	field_types.insert({ 237, { "postMplsTopLabelExp", dt_unsigned8 } });
	field_types.insert({ 238, { "tcpWindowScale", dt_unsigned16 } });
	field_types.insert({ 239, { "biflowDirection", dt_unsigned8 } });
	field_types.insert({ 240, { "ethernetHeaderLength", dt_unsigned8 } });
	field_types.insert({ 241, { "ethernetPayloadLength", dt_unsigned16 } });
	field_types.insert({ 242, { "ethernetTotalLength", dt_unsigned16 } });
	field_types.insert({ 243, { "dot1qVlanId", dt_unsigned16 } });
	field_types.insert({ 244, { "dot1qPriority", dt_unsigned8 } });
	field_types.insert({ 245, { "dot1qCustomerVlanId", dt_unsigned16 } });
	field_types.insert({ 246, { "dot1qCustomerPriority", dt_unsigned8 } });
	field_types.insert({ 247, { "metroEvcId", dt_string } });
	field_types.insert({ 248, { "metroEvcType", dt_unsigned8 } });
	field_types.insert({ 249, { "pseudoWireId", dt_unsigned32 } });
	field_types.insert({ 250, { "pseudoWireType", dt_unsigned16 } });
	field_types.insert({ 251, { "pseudoWireControlWord", dt_unsigned32 } });
	field_types.insert({ 252, { "ingressPhysicalInterface", dt_unsigned32 } });
	field_types.insert({ 253, { "egressPhysicalInterface", dt_unsigned32 } });
	field_types.insert({ 254, { "postDot1qVlanId", dt_unsigned16 } });
	field_types.insert({ 255, { "postDot1qCustomerVlanId", dt_unsigned16 } });
	field_types.insert({ 256, { "ethernetType", dt_unsigned16 } });
	field_types.insert({ 257, { "postIpPrecedence", dt_unsigned8 } });
	field_types.insert({ 258, { "collectionTimeMilliseconds", dt_dateTimeMilliseconds } });
	field_types.insert({ 259, { "exportSctpStreamId", dt_unsigned16 } });
	field_types.insert({ 260, { "maxExportSeconds", dt_dateTimeSeconds } });
	field_types.insert({ 261, { "maxFlowEndSeconds", dt_dateTimeSeconds } });
	field_types.insert({ 262, { "messageMD5Checksum", dt_octetArray } });
	field_types.insert({ 263, { "messageScope", dt_unsigned8 } });
	field_types.insert({ 264, { "minExportSeconds", dt_dateTimeSeconds } });
	field_types.insert({ 265, { "minFlowStartSeconds", dt_dateTimeSeconds } });
	field_types.insert({ 266, { "opaqueOctets", dt_octetArray } });
	field_types.insert({ 267, { "sessionScope", dt_unsigned8 } });
	field_types.insert({ 268, { "maxFlowEndMicroseconds", dt_dateTimeMicroseconds } });
	field_types.insert({ 269, { "maxFlowEndMilliseconds", dt_dateTimeMilliseconds } });
	field_types.insert({ 270, { "maxFlowEndNanoseconds", dt_dateTimeNanoseconds } });
	field_types.insert({ 271, { "minFlowStartMicroseconds", dt_dateTimeMicroseconds } });
	field_types.insert({ 272, { "minFlowStartMilliseconds", dt_dateTimeMilliseconds } });
	field_types.insert({ 273, { "minFlowStartNanoseconds", dt_dateTimeNanoseconds } });
	field_types.insert({ 274, { "collectorCertificate", dt_octetArray } });
	field_types.insert({ 275, { "exporterCertificate", dt_octetArray } });
	field_types.insert({ 276, { "dataRecordsReliability", dt_boolean } });
	field_types.insert({ 277, { "observationPointType", dt_unsigned8 } });
	field_types.insert({ 278, { "newConnectionDeltaCount", dt_unsigned32 } });
	field_types.insert({ 279, { "connectionSumDurationSeconds", dt_unsigned64 } });
	field_types.insert({ 280, { "connectionTransactionId", dt_unsigned64 } });
	field_types.insert({ 281, { "postNATSourceIPv6Address", dt_ipv6Address } });
	field_types.insert({ 282, { "postNATDestinationIPv6Address", dt_ipv6Address } });
	field_types.insert({ 283, { "natPoolId", dt_unsigned32 } });
	field_types.insert({ 284, { "natPoolName", dt_string } });
	field_types.insert({ 285, { "anonymizationFlags", dt_unsigned16 } });
	field_types.insert({ 286, { "anonymizationTechnique", dt_unsigned16 } });
	field_types.insert({ 287, { "informationElementIndex", dt_unsigned16 } });
	field_types.insert({ 288, { "p2pTechnology", dt_string } });
	field_types.insert({ 289, { "tunnelTechnology", dt_string } });
	field_types.insert({ 290, { "encryptedTechnology", dt_string } });
	field_types.insert({ 291, { "basicList", dt_basicList } });
	field_types.insert({ 292, { "subTemplateList", dt_subTemplateList } });
	field_types.insert({ 293, { "subTemplateMultiList", dt_subTemplateMultiList } });
	field_types.insert({ 294, { "bgpValidityState", dt_unsigned8 } });
	field_types.insert({ 295, { "IPSecSPI", dt_unsigned32 } });
	field_types.insert({ 296, { "greKey", dt_unsigned32 } });
	field_types.insert({ 297, { "natType", dt_unsigned8 } });
	field_types.insert({ 298, { "initiatorPackets", dt_unsigned64 } });
	field_types.insert({ 299, { "responderPackets", dt_unsigned64 } });
	field_types.insert({ 300, { "observationDomainName", dt_string } });
	field_types.insert({ 301, { "selectionSequenceId", dt_unsigned64 } });
	field_types.insert({ 302, { "selectorId", dt_unsigned64 } });
	field_types.insert({ 303, { "informationElementId", dt_unsigned16 } });
	field_types.insert({ 304, { "selectorAlgorithm", dt_unsigned16 } });
	field_types.insert({ 305, { "samplingPacketInterval", dt_unsigned32 } });
	field_types.insert({ 306, { "samplingPacketSpace", dt_unsigned32 } });
	field_types.insert({ 307, { "samplingTimeInterval", dt_unsigned32 } });
	field_types.insert({ 308, { "samplingTimeSpace", dt_unsigned32 } });
	field_types.insert({ 309, { "samplingSize", dt_unsigned32 } });
	field_types.insert({ 310, { "samplingPopulation", dt_unsigned32 } });
	field_types.insert({ 311, { "samplingProbability", dt_float64 } });
	field_types.insert({ 312, { "dataLinkFrameSize", dt_unsigned16 } });
	field_types.insert({ 313, { "ipHeaderPacketSection", dt_octetArray } });
	field_types.insert({ 314, { "ipPayloadPacketSection", dt_octetArray } });
	field_types.insert({ 315, { "dataLinkFrameSection", dt_octetArray } });
	field_types.insert({ 316, { "mplsLabelStackSection", dt_octetArray } });
	field_types.insert({ 317, { "mplsPayloadPacketSection", dt_octetArray } });
	field_types.insert({ 318, { "selectorIdTotalPktsObserved", dt_unsigned64 } });
	field_types.insert({ 319, { "selectorIdTotalPktsSelected", dt_unsigned64 } });
	field_types.insert({ 320, { "absoluteError", dt_float64 } });
	field_types.insert({ 321, { "relativeError", dt_float64 } });
	field_types.insert({ 322, { "observationTimeSeconds", dt_dateTimeSeconds } });
	field_types.insert({ 323, { "observationTimeMilliseconds", dt_dateTimeMilliseconds } });
	field_types.insert({ 324, { "observationTimeMicroseconds", dt_dateTimeMicroseconds } });
	field_types.insert({ 325, { "observationTimeNanoseconds", dt_dateTimeNanoseconds } });
	field_types.insert({ 326, { "digestHashValue", dt_unsigned64 } });
	field_types.insert({ 327, { "hashIPPayloadOffset", dt_unsigned64 } });
	field_types.insert({ 328, { "hashIPPayloadSize", dt_unsigned64 } });
	field_types.insert({ 329, { "hashOutputRangeMin", dt_unsigned64 } });
	field_types.insert({ 330, { "hashOutputRangeMax", dt_unsigned64 } });
	field_types.insert({ 331, { "hashSelectedRangeMin", dt_unsigned64 } });
	field_types.insert({ 332, { "hashSelectedRangeMax", dt_unsigned64 } });
	field_types.insert({ 333, { "hashDigestOutput", dt_boolean } });
	field_types.insert({ 334, { "hashInitialiserValue", dt_unsigned64 } });
	field_types.insert({ 335, { "selectorName", dt_string } });
	field_types.insert({ 336, { "upperCILimit", dt_float64 } });
	field_types.insert({ 337, { "lowerCILimit", dt_float64 } });
	field_types.insert({ 338, { "confidenceLevel", dt_float64 } });
	field_types.insert({ 339, { "informationElementDataType", dt_unsigned8 } });
	field_types.insert({ 340, { "informationElementDescription", dt_string } });
	field_types.insert({ 341, { "informationElementName", dt_string } });
	field_types.insert({ 342, { "informationElementRangeBegin", dt_unsigned64 } });
	field_types.insert({ 343, { "informationElementRangeEnd", dt_unsigned64 } });
	field_types.insert({ 344, { "informationElementSemantics", dt_unsigned8 } });
	field_types.insert({ 345, { "informationElementUnits", dt_unsigned16 } });
	field_types.insert({ 346, { "privateEnterpriseNumber", dt_unsigned32 } });
	field_types.insert({ 347, { "virtualStationInterfaceId", dt_octetArray } });
	field_types.insert({ 348, { "virtualStationInterfaceName", dt_string } });
	field_types.insert({ 349, { "virtualStationUUID", dt_octetArray } });
	field_types.insert({ 350, { "virtualStationName", dt_string } });
	field_types.insert({ 351, { "layer2SegmentId", dt_unsigned64 } });
	field_types.insert({ 352, { "layer2OctetDeltaCount", dt_unsigned64 } });
	field_types.insert({ 353, { "layer2OctetTotalCount", dt_unsigned64 } });
	field_types.insert({ 354, { "ingressUnicastPacketTotalCount", dt_unsigned64 } });
	field_types.insert({ 355, { "ingressMulticastPacketTotalCount", dt_unsigned64 } });
	field_types.insert({ 356, { "ingressBroadcastPacketTotalCount", dt_unsigned64 } });
	field_types.insert({ 357, { "egressUnicastPacketTotalCount", dt_unsigned64 } });
	field_types.insert({ 358, { "egressBroadcastPacketTotalCount", dt_unsigned64 } });
	field_types.insert({ 359, { "monitoringIntervalStartMilliSeconds", dt_dateTimeMilliseconds } });
	field_types.insert({ 360, { "monitoringIntervalEndMilliSeconds", dt_dateTimeMilliseconds } });
	field_types.insert({ 361, { "portRangeStart", dt_unsigned16 } });
	field_types.insert({ 362, { "portRangeEnd", dt_unsigned16 } });
	field_types.insert({ 363, { "portRangeStepSize", dt_unsigned16 } });
	field_types.insert({ 364, { "portRangeNumPorts", dt_unsigned16 } });
	field_types.insert({ 365, { "staMacAddress", dt_macAddress } });
	field_types.insert({ 366, { "staIPv4Address", dt_ipv4Address } });
	field_types.insert({ 367, { "wtpMacAddress", dt_macAddress } });
	field_types.insert({ 368, { "ingressInterfaceType", dt_unsigned32 } });
	field_types.insert({ 369, { "egressInterfaceType", dt_unsigned32 } });
	field_types.insert({ 370, { "rtpSequenceNumber", dt_unsigned16 } });
	field_types.insert({ 371, { "userName", dt_string } });
	field_types.insert({ 372, { "applicationCategoryName", dt_string } });
	field_types.insert({ 373, { "applicationSubCategoryName", dt_string } });
	field_types.insert({ 374, { "applicationGroupName", dt_string } });
	field_types.insert({ 375, { "originalFlowsPresent", dt_unsigned64 } });
	field_types.insert({ 376, { "originalFlowsInitiated", dt_unsigned64 } });
	field_types.insert({ 377, { "originalFlowsCompleted", dt_unsigned64 } });
	field_types.insert({ 378, { "distinctCountOfSourceIPAddress", dt_unsigned64 } });
	field_types.insert({ 379, { "distinctCountOfDestinationIPAddress", dt_unsigned64 } });
	field_types.insert({ 380, { "distinctCountOfSourceIPv4Address", dt_unsigned32 } });
	field_types.insert({ 381, { "distinctCountOfDestinationIPv4Address", dt_unsigned32 } });
	field_types.insert({ 382, { "distinctCountOfSourceIPv6Address", dt_unsigned64 } });
	field_types.insert({ 383, { "distinctCountOfDestinationIPv6Address", dt_unsigned64 } });
	field_types.insert({ 384, { "valueDistributionMethod", dt_unsigned8 } });
	field_types.insert({ 385, { "rfc3550JitterMilliseconds", dt_unsigned32 } });
	field_types.insert({ 386, { "rfc3550JitterMicroseconds", dt_unsigned32 } });
	field_types.insert({ 387, { "rfc3550JitterNanoseconds", dt_unsigned32 } });
	field_types.insert({ 388, { "dot1qDEI", dt_boolean } });
	field_types.insert({ 389, { "dot1qCustomerDEI", dt_boolean } });
	field_types.insert({ 390, { "flowSelectorAlgorithm", dt_unsigned16 } });
	field_types.insert({ 391, { "flowSelectedOctetDeltaCount", dt_unsigned64 } });
	field_types.insert({ 392, { "flowSelectedPacketDeltaCount", dt_unsigned64 } });
	field_types.insert({ 393, { "flowSelectedFlowDeltaCount", dt_unsigned64 } });
	field_types.insert({ 394, { "selectorIDTotalFlowsObserved", dt_unsigned64 } });
	field_types.insert({ 395, { "selectorIDTotalFlowsSelected", dt_unsigned64 } });
	field_types.insert({ 396, { "samplingFlowInterval", dt_unsigned64 } });
	field_types.insert({ 397, { "samplingFlowSpacing", dt_unsigned64 } });
	field_types.insert({ 398, { "flowSamplingTimeInterval", dt_unsigned64 } });
	field_types.insert({ 399, { "flowSamplingTimeSpacing", dt_unsigned64 } });
	field_types.insert({ 400, { "hashFlowDomain", dt_unsigned16 } });
	field_types.insert({ 401, { "transportOctetDeltaCount", dt_unsigned64 } });
	field_types.insert({ 402, { "transportPacketDeltaCount", dt_unsigned64 } });
	field_types.insert({ 403, { "originalExporterIPv4Address", dt_ipv4Address } });
	field_types.insert({ 404, { "originalExporterIPv6Address", dt_ipv6Address } });
	field_types.insert({ 405, { "originalObservationDomainId", dt_unsigned32 } });
	field_types.insert({ 406, { "intermediateProcessId", dt_unsigned32 } });
	field_types.insert({ 407, { "ignoredDataRecordTotalCount", dt_unsigned64 } });
	field_types.insert({ 408, { "dataLinkFrameType", dt_unsigned16 } });
	field_types.insert({ 409, { "sectionOffset", dt_unsigned16 } });
	field_types.insert({ 410, { "sectionExportedOctets", dt_unsigned16 } });
	field_types.insert({ 411, { "dot1qServiceInstanceTag", dt_octetArray } });
	field_types.insert({ 412, { "dot1qServiceInstanceId", dt_unsigned32 } });
	field_types.insert({ 413, { "dot1qServiceInstancePriority", dt_unsigned8 } });
	field_types.insert({ 414, { "dot1qCustomerSourceMacAddress", dt_macAddress } });
	field_types.insert({ 415, { "dot1qCustomerDestinationMacAddress", dt_macAddress } });
	field_types.insert({ 416, { "", dt_reserved } });
	field_types.insert({ 417, { "postLayer2OctetDeltaCount", dt_unsigned64 } });
	field_types.insert({ 418, { "postMCastLayer2OctetDeltaCount", dt_unsigned64 } });
	field_types.insert({ 419, { "", dt_reserved } });
	field_types.insert({ 420, { "postLayer2OctetTotalCount", dt_unsigned64 } });
	field_types.insert({ 421, { "postMCastLayer2OctetTotalCount", dt_unsigned64 } });
	field_types.insert({ 422, { "minimumLayer2TotalLength", dt_unsigned64 } });
	field_types.insert({ 423, { "maximumLayer2TotalLength", dt_unsigned64 } });
	field_types.insert({ 424, { "droppedLayer2OctetDeltaCount", dt_unsigned64 } });
	field_types.insert({ 425, { "droppedLayer2OctetTotalCount", dt_unsigned64 } });
	field_types.insert({ 426, { "ignoredLayer2OctetTotalCount", dt_unsigned64 } });
	field_types.insert({ 427, { "notSentLayer2OctetTotalCount", dt_unsigned64 } });
	field_types.insert({ 428, { "layer2OctetDeltaSumOfSquares", dt_unsigned64 } });
	field_types.insert({ 429, { "layer2OctetTotalSumOfSquares", dt_unsigned64 } });
	field_types.insert({ 430, { "layer2FrameDeltaCount", dt_unsigned64 } });
	field_types.insert({ 431, { "layer2FrameTotalCount", dt_unsigned64 } });
	field_types.insert({ 432, { "pseudoWireDestinationIPv4Address", dt_ipv4Address } });
	field_types.insert({ 433, { "ignoredLayer2FrameTotalCount", dt_unsigned64 } });
	field_types.insert({ 434, { "mibObjectValueInteger", dt_signed32 } });
	field_types.insert({ 435, { "mibObjectValueOctetString", dt_octetArray } });
	field_types.insert({ 436, { "mibObjectValueOID", dt_octetArray } });
	field_types.insert({ 437, { "mibObjectValueBits", dt_octetArray } });
	field_types.insert({ 438, { "mibObjectValueIPAddress", dt_ipv4Address } });
	field_types.insert({ 439, { "mibObjectValueCounter", dt_unsigned64 } });
	field_types.insert({ 440, { "mibObjectValueGauge", dt_unsigned32 } });
	field_types.insert({ 441, { "mibObjectValueTimeTicks", dt_unsigned32 } });
	field_types.insert({ 442, { "mibObjectValueUnsigned", dt_unsigned32 } });
	field_types.insert({ 443, { "mibObjectValueTable", dt_subTemplateList } });
	field_types.insert({ 444, { "mibObjectValueRow", dt_subTemplateList } });
	field_types.insert({ 445, { "mibObjectIdentifier", dt_octetArray } });
	field_types.insert({ 446, { "mibSubIdentifier", dt_unsigned32 } });
	field_types.insert({ 447, { "mibIndexIndicator", dt_unsigned64 } });
	field_types.insert({ 448, { "mibCaptureTimeSemantics", dt_unsigned8 } });
	field_types.insert({ 449, { "mibContextEngineID", dt_octetArray } });
	field_types.insert({ 450, { "mibContextName", dt_string } });
	field_types.insert({ 451, { "mibObjectName", dt_string } });
	field_types.insert({ 452, { "mibObjectDescription", dt_string } });
	field_types.insert({ 453, { "mibObjectSyntax", dt_string } });
	field_types.insert({ 454, { "mibModuleName", dt_string } });
	field_types.insert({ 455, { "mobileIMSI", dt_string } });
	field_types.insert({ 456, { "mobileMSISDN", dt_string } });
	field_types.insert({ 457, { "httpStatusCode", dt_unsigned16 } });
	field_types.insert({ 458, { "sourceTransportPortsLimit", dt_unsigned16 } });
	field_types.insert({ 459, { "httpRequestMethod", dt_string } });
	field_types.insert({ 460, { "httpRequestHost", dt_string } });
	field_types.insert({ 461, { "httpRequestTarget", dt_string } });
	field_types.insert({ 462, { "httpMessageVersion", dt_string } });
	field_types.insert({ 463, { "natInstanceID", dt_unsigned32 } });
	field_types.insert({ 464, { "internalAddressRealm", dt_octetArray } });
	field_types.insert({ 465, { "externalAddressRealm", dt_octetArray } });
	field_types.insert({ 466, { "natQuotaExceededEvent", dt_unsigned32 } });
	field_types.insert({ 467, { "natThresholdEvent", dt_unsigned32 } });
	field_types.insert({ 468, { "httpUserAgent", dt_string } });
	field_types.insert({ 469, { "httpContentType", dt_string } });
	field_types.insert({ 470, { "httpReasonPhrase", dt_string } });
	field_types.insert({ 471, { "maxSessionEntries", dt_unsigned32 } });
	field_types.insert({ 472, { "maxBIBEntries", dt_unsigned32 } });
	field_types.insert({ 473, { "maxEntriesPerUser", dt_unsigned32 } });
	field_types.insert({ 474, { "maxSubscribers", dt_unsigned32 } });
	field_types.insert({ 475, { "maxFragmentsPendingReassembly", dt_unsigned32 } });
	field_types.insert({ 476, { "addressPoolHighThreshold", dt_unsigned32 } });
	field_types.insert({ 477, { "addressPoolLowThreshold", dt_unsigned32 } });
	field_types.insert({ 478, { "addressPortMappingHighThreshold", dt_unsigned32 } });
	field_types.insert({ 479, { "addressPortMappingLowThreshold", dt_unsigned32 } });
	field_types.insert({ 480, { "addressPortMappingPerUserHighThreshold", dt_unsigned32 } });
	field_types.insert({ 481, { "globalAddressMappingHighThreshold", dt_unsigned32 } });
	field_types.insert({ 482, { "vpnIdentifier", dt_octetArray } });
	field_types.insert({ 483, { "bgpCommunity", dt_unsigned32 } });
	field_types.insert({ 484, { "bgpSourceCommunityList", dt_basicList } });
	field_types.insert({ 485, { "bgpDestinationCommunityList", dt_basicList } });
	field_types.insert({ 486, { "bgpExtendedCommunity", dt_octetArray } });
	field_types.insert({ 487, { "bgpSourceExtendedCommunityList", dt_basicList } });
	field_types.insert({ 488, { "bgpDestinationExtendedCommunityList", dt_basicList } });
	field_types.insert({ 489, { "bgpLargeCommunity", dt_octetArray } });
	field_types.insert({ 490, { "bgpSourceLargeCommunityList", dt_basicList } });
	field_types.insert({ 491, { "bgpDestinationLargeCommunityList", dt_basicList } });
}

ipfix::~ipfix()
{
	templates.clear();
	field_types.clear();
}

bool ipfix::process_packet(const uint8_t *const packet, const int packet_size, db *const target)
{
	buffer b(packet, packet_size);

	dolog(ll_debug, "process_ipfix_packet: packet size          : %d", packet_size);

	// message header
	uint16_t version_number        = b.get_net_short();
	uint16_t length                = b.get_net_short();
	time_t   export_time           = b.get_net_long ();
	uint32_t sequence_number       = b.get_net_long ();
	uint32_t observation_domain_id = b.get_net_long ();

	dolog(ll_debug, "process_ipfix_packet: version number       : %d", version_number);
	dolog(ll_debug, "process_ipfix_packet: length               : %d", length);
	dolog(ll_debug, "process_ipfix_packet: export time          : %ld", export_time);
	dolog(ll_debug, "process_ipfix_packet: sequence number      : %d", sequence_number);
	dolog(ll_debug, "process_ipfix_packet: observation domain id: %d", observation_domain_id);

	while(b.end_reached() == false) {
		// set-header
		uint16_t set_id     = b.get_net_short();
		uint16_t set_length = b.get_net_short();

		dolog(ll_debug, "process_ipfix_packet: set id    : %d", set_id);
		dolog(ll_debug, "process_ipfix_packet: set length: %d", set_length);

		if (set_length < 4) {
			dolog(ll_debug, "process_ipfix_packet: set length invalid, must be at least 4");
			return false;
		}

		buffer set = b.get_segment(set_length - 4);

		if (set_id == 0 || set_id == 1) {  // not used (RFC3954)
			dolog(ll_debug, "process_ipfix_packet: set id 0 and 1 should not occur");

			return false;
		}
		else if (set_id == 2) {  // template record
			uint16_t template_id = set.get_net_short();
			uint16_t field_count = set.get_net_short();

			dolog(ll_debug, "process_ipfix_packet: template id: %d", template_id);
			dolog(ll_debug, "process_ipfix_packet: field count: %d", field_count);

			std::vector<information_element_t> template_;

			for(uint16_t nr=0; nr<field_count; nr++) {
				uint16_t ie_identifier     = set.get_net_short();  // information element identifier
				bool     enterprise        = !!(ie_identifier & 32768);
				uint16_t field_length      = set.get_net_short();
				uint32_t enterprise_number = 0;

				ie_identifier &= 32767;

				if (enterprise)
					enterprise_number  = set.get_net_long();

				information_element_t ie { 0 };
				ie.enterprise                     = enterprise;
				ie.information_element_identifier = ie_identifier;
				ie.field_length                   = field_length;
				ie.enterprise_number              = enterprise_number;

				if (enterprise)
					dolog(ll_debug, "process_ipfix_packet: field %d is type %d and is enterprise, length is %d bytes (enterprise number: %d)", nr, ie_identifier, field_length, enterprise_number);
				else
					dolog(ll_debug, "process_ipfix_packet: field %d is type %d and is not enterprise, length is %d bytes", nr, ie_identifier, field_length);

				template_.push_back(ie);
			}

			templates.insert_or_assign(template_id, template_);

			if (set.end_reached() == false) {
				dolog(ll_warning, "process_ipfix_packet: data (set) underflow (%d bytes left)", set.get_n_bytes_left());

				return false;
			}
		}
		else if (set_id == 3) {  // options template sets
			dolog(ll_warning, "process_ipfix_packet: options template sets not implemented");

			// return false;
		}
		else if (set_id >= 256) {  // data sets
			auto data_set = templates.find(set_id);

			if (data_set == templates.end()) {
				dolog(ll_debug, "process_ipfix_packet: template %d not set (yet)", set_id);

				return false;
			}

			dolog(ll_debug, "process_ipfix_packet: template %d has %zu elements", set_id, data_set->second.size());

			db_record_t db_record;
			db_record.export_time           = export_time;
			db_record.sequence_number       = sequence_number;
			db_record.observation_domain_id = observation_domain_id;

			for(auto field : data_set->second) {
				auto it = field_types.find(field.information_element_identifier);

				if (it == field_types.end()) {
					dolog(ll_warning, "process_ipfix_packet: information element identifier %d is not known", field.information_element_identifier);

					return false;
				}

				// value, type of value (e.g. int, float, string), length of it
				db_record_data_t drd { set.get_segment(field.field_length), it->second.second, field.field_length };

				if (log_enabled(ll_debug)) {
					buffer copy = drd.b;

					std::optional<std::string> data = data_to_str(it->second.second, field.field_length, copy);

					if (data.has_value() == false) {
						dolog(ll_debug, "process_ipfix_packet: information element %s of type %d: cannot convert, type not supported or invalid data", it->second.first.c_str(), it->second.second);

						return false;
					}

					dolog(ll_debug, "process_ipfix_packet: information element %s of type %d: \"%s\"", it->second.first.c_str(), it->second.second, data.value().c_str());
				}

				db_record.data.insert({ it->second.first, drd });
			}

			if (target)
				target->insert(db_record);
		}
		else {
			dolog(ll_error, "process_ipfix_packet: set type %d not implemented", set_id);
		}
	}

	if (b.end_reached() == false) {
		dolog(ll_warning, "process_ipfix_packet: data (packet) underflow (%d bytes left)", b.get_n_bytes_left());

		return false;
	}

	return true;
}

std::optional<std::string> ipfix::data_to_str(const data_type_t & type, const int len, buffer & data_source)
{
	std::optional<std::string> out = { };
	int expected_length = -1;

	switch(type) {
		case dt_octetArray: {
				expected_length = len;

				std::string temp;

				for(int i=0; i<len; i++)
					temp += myformat("%02x", data_source.get_byte());

				out = temp;
		    	}
			break;

		case dt_unsigned8:
			expected_length = 1;
			out = myformat("%u", data_source.get_byte());
			break;

		case dt_unsigned16:
			expected_length = len <= 2 ? len : 2;
			out = myformat("%u", uint16_t(get_variable_size_integer(data_source, len)));
			break;

		case dt_unsigned32:
			expected_length = len <= 4 ? len : 4;
			out = myformat("%u", uint32_t(get_variable_size_integer(data_source, len)));
			break;

		case dt_unsigned64:
			expected_length = len <= 8 ? len : 8;
			out = myformat("%lu", get_variable_size_integer(data_source, len));
			break;

		case dt_signed8:
			expected_length = 1;
			out = myformat("%d", static_cast<int8_t>(data_source.get_byte()));
			break;

		case dt_signed16:
			expected_length = 2;
			out = myformat("%d", static_cast<int16_t>(data_source.get_net_short()));
			break;

		case dt_signed32:
			expected_length = 4;
			out = myformat("%d", static_cast<int32_t>(data_source.get_net_long()));
			break;

		case dt_signed64:
			expected_length = 8;
			out = myformat("%ld", static_cast<int64_t>(data_source.get_net_long_long()));
			break;

		case dt_float32: {
				expected_length = 4;
				uint32_t temp = data_source.get_net_long();
				out = myformat("%f", *reinterpret_cast<float *>(&temp));
			}
			break;

		case dt_float64: {
				expected_length = 8;
				uint64_t temp = data_source.get_net_long();
				out = myformat("%f", *reinterpret_cast<double *>(&temp));
			}
			break;

		case dt_boolean: {
				expected_length = 8;
				uint8_t v = data_source.get_byte();

				if (v == 1)
					out = "true";
				else if (v == 2)
					out = "false";
				else
					dolog(ll_warning, "ipfix::data_to_str: unexpected value %d found for type %d, expected 1 or 2", v, type);
			}
			break;

		case dt_macAddress: {
				expected_length = 6;

				uint8_t bytes[6] { 0 };
				for(int i=0; i<6; i++)
					bytes[i] = data_source.get_byte();

				out = myformat("%02x:%02x:%02x:%02x:%02x:%02x", bytes[0], bytes[1], bytes[2], bytes[3], bytes[4], bytes[5]);
			}
			break;

		case dt_string:
			expected_length = len;
			out = data_source.get_string(len);
			break;

		case dt_dateTimeSeconds:
			expected_length = 4;
			out = myformat("%u", data_source.get_net_long());
			break;

		case dt_dateTimeMilliseconds:
			expected_length = 8;
			out = myformat("%lu", data_source.get_net_long_long());
			break;

		case dt_dateTimeMicroseconds:
			expected_length = 8;
			out = myformat("%lu", data_source.get_net_long_long());
			break;

		case dt_dateTimeNanoseconds:
			expected_length = 8;
			out = myformat("%lu", data_source.get_net_long_long());
			break;

		case dt_ipv4Address: {
				expected_length = 4;

				uint8_t bytes[4] { 0 };
				for(int i=0; i<4; i++)
					bytes[i] = data_source.get_byte();

				out = myformat("%d.%d.%d.%d", bytes[0], bytes[1], bytes[2], bytes[3]);
			}
			break;

		case dt_ipv6Address: {
				expected_length = 16;

				uint8_t bytes[16] { 0 };
				for(int i=0; i<16; i++)
					bytes[i] = data_source.get_byte();

				out = myformat("%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
						bytes[0], bytes[1], bytes[2], bytes[3], bytes[4], bytes[5], bytes[6], bytes[7],
						bytes[8], bytes[9], bytes[10], bytes[11], bytes[12], bytes[13], bytes[14], bytes[15]);
			}
			break;

		// case dt_basicList,
		// case dt_subTemplateList,
		// case dt_subTemplateMultiList

		default:
			data_source.seek(len);
			break;
	}

	if (expected_length != len) {
		dolog(ll_warning, "ipfix::data_to_str: unexpected length %d found for type %d, expected %d", len, type, expected_length);
		return { };
	}

	return out;
}
