# Examples Overview

* **common_ressources** contains topic and geometry descriptions used by all examples.
* **basic** is an unicast example with static subscriptions.
* **discovery** in a multicast example with dynamic subscription discovery.
* **location_filter** shows the filtering mechanism based on the publishers location.

All examples are available for Linux and RIOT-OS. For Linux you can setup the pre-configured interfaces by using **setup_tap_ifaces.sh**. 
For RIOT-OS you have to specify the IPs in the **sdds.xml**.

You can also create your own examples by edditing the **examples.xml** and generating the new examples wit **generate_examples.sh**
