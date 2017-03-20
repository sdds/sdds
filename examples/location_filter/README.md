# Basic Example

This Example shows a location filter. 

It consists of a publisher, filtered subscriber, a lication tracking simulation and a subscription manager.
* The publisher **temperature_pub** publishes temperature samples on a temperature topic. 
* The subscriber sibscribes the topic with a location filter.
* The samples are disseminated by the middleware to the discovered subscriber. Incoming samples which do not meet the filter are discarded.
* The location tracking simulation publishes location informations.
* The subscription manager observes active subscriptions and pauses subscriptions based on the filters set for the subscription
