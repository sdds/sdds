# Basic Example

This Example shows a basic subscription discovery. 

It consists of a publisher and two different subscriber versions (use only one at a time).
* The publisher **temperature_pub** publishes temperature samples on a temperature topic. 
* The samples are disseminated by the middleware to the discovered subscriber.
 *Both subscribers subscribe to the temperature topic and show the different possibilities to handle received samples.
* The subscriber **temperature_sub_polling** shows a polling example while the subscriber **temperature_sub_listener** shows how to register a callback.
