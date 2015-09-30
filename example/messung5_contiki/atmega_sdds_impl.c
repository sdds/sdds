#include "atmega_sdds_impl.h"

#include <os-ssal/Memory.h>
#include <sdds/DataSink.h>
#include <sdds/DataSource.h>
#include <sdds/LocatorDB.h>
#include <sdds/Log.h>
#include <sdds/Network.h>
#include <sdds/sDDS.h>
/*#include <sdds_conn.h>*/


DDS_ReturnCode_t DDS_DataReader_set_listener(
	DDS_DataReader _this,
	const struct DDS_DataReaderListener* a_listener,
	const DDS_StatusMask mask
)
{
	rc_t ret = DataSink_set_on_data_avail_listener((DataReader) _this, (On_Data_Avail_Listener) a_listener->on_data_available, (const StatusMask) mask);
	if (ret == SDDS_RT_OK)
		return DDS_RETCODE_OK;

	return DDS_RETCODE_ERROR;
}

DDS_Topic g_Alpha_topic;
Alpha g_Alpha_pool[SDDS_TOPIC_APP_MSG_COUNT];
DDS_DataWriter g_Alpha_writer;

DDS_Topic g_Beta_topic;
Beta g_Beta_pool[SDDS_TOPIC_APP_MSG_COUNT];
DDS_DataWriter g_Beta_writer;

rc_t sDDS_init(void)
{
	rc_t ret;
	
	Memory_init();
	LocatorDB_init();
	DataSource_init();
	DataSink_init();
	Network_init();
#ifdef FEATURE_SDDS_BUILTIN_TOPICS_ENABLED
	BuiltinTopic_init();
#endif
#ifdef	FEATURE_SDDS_DISCOVERY_ENABLED
	Discovery_init();
#endif

    int log_level = 3;

	Log_setLvl(log_level);

	Locator l;
	g_Alpha_topic = sDDS_AlphaTopic_create(g_Alpha_pool, SDDS_TOPIC_APP_MSG_COUNT);
	g_Alpha_writer = DataSource_create_datawriter(g_Alpha_topic, NULL, NULL, NULL);


	ret = LocatorDB_newLocator(&l);
	if (ret != SDDS_RT_OK)
		return ret;

	Locator_upRef(l);

	ret = Network_setAddressToLocator(l, "fe80::12:13ff:fe14:1516");
	if (ret != SDDS_RT_OK)
		return ret;

	ret = Topic_addRemoteDataSink(g_Alpha_topic, l);
	if (ret != SDDS_RT_OK)
		return ret;
	Locator_downRef(l);

	g_Beta_topic = sDDS_BetaTopic_create(g_Beta_pool, SDDS_TOPIC_APP_MSG_COUNT);
	g_Beta_writer = DataSource_create_datawriter(g_Beta_topic, NULL, NULL, NULL);


	ret = LocatorDB_newLocator(&l);
	if (ret != SDDS_RT_OK)
		return ret;

	Locator_upRef(l);

	ret = Network_setAddressToLocator(l, "fe80::12:13ff:fe14:1516");
	if (ret != SDDS_RT_OK)
		return ret;

	ret = Topic_addRemoteDataSink(g_Beta_topic, l);
	if (ret != SDDS_RT_OK)
		return ret;
	Locator_downRef(l);

	return SDDS_RT_OK;
}
