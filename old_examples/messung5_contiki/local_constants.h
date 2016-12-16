#define NETSTACK_CONF_WITH_IPV6 1
#define UIP_CONF_ROUTER			1
#define	UIP_CONF_IPV6_MULTICAST 1
#define UIP_CONF_IPV6_RPL		1

#include "net/ipv6/multicast/uip-mcast6-engines.h"

/* Change this to switch engines. Engine codes in uip-mcast6-engines.h */
#define UIP_MCAST6_CONF_ENGINE UIP_MCAST6_ENGINE_ROLL_TM

/* For Imin: Use 16 over NullRDC, 64 over Contiki MAC */
#define ROLL_TM_CONF_IMIN_1         64

#undef UIP_CONF_IPV6_RPL
#undef UIP_CONF_ND6_SEND_RA
#undef UIP_CONF_ROUTER
#define UIP_CONF_ND6_SEND_RA         0
#define UIP_CONF_ROUTER              1
#define UIP_MCAST6_ROUTE_CONF_ROUTES 1

/* Code/RAM footprint savings so that things will fit on our device */
#undef UIP_CONF_DS6_NBR_NBU
#undef UIP_CONF_DS6_ROUTE_NBU
#define UIP_CONF_DS6_NBR_NBU        10
#define UIP_CONF_DS6_ROUTE_NBU      10

//#define UTILS_DEBUG

#define FEATURE_SDDS_MULTICAST_ENABLED
#define FEATURE_SDDS_BUILTIN_TOPICS_ENABLED
#define FEATURE_SDDS_DISCOVERY_ENABLED

#define SDDS_NET_MAX_LOCATOR_COUNT 					20
#define SDDS_DISCOVERY_RECEIVE_TIMER				1
#define SDDS_DISCOVERY_PARTICIPANT_TIMER			10
#define SDDS_DISCOVERY_PUBLICATION_TIMER			60
#define SDDS_DISCOVERY_TOPIC_TIMER					0

#define SDDS_BUILTIN_MULTICAST_ADDRESS				"ff02::10"
#define SDDS_BUILTIN_PARTICIPANT_ADDRESS			"ff02::20"
#define SDDS_BUILTIN_SUB_PUB_ADDRESS				"ff02::20"
#define SDDS_BUILTIN_TOPIC_ADDRESS					"ff02::20"
