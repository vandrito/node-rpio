/*
 * Copyright (c) 2012 Jonathan Perkin <jonathan@perkin.org.uk>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <node.h>
#include <node_buffer.h>
#include <v8.h>

#include "bcm2835.h"

using namespace v8;

/*
 * Set a pin as input
 */
Handle<Value> SetInput(const Arguments& args)
{
	HandleScope scope;
#ifdef __arm__
	if (args.Length() != 1) {
		ThrowException(Exception::TypeError(String::New("Incorrect number of arguments")));
		return scope.Close(Undefined());
	}

	if (!args[0]->IsNumber()) {
		ThrowException(Exception::TypeError(String::New("Incorrect argument type(s)")));
		return scope.Close(Undefined());
	}

	bcm2835_gpio_fsel(args[0]->NumberValue(), BCM2835_GPIO_FSEL_INPT);

	return scope.Close(Integer::New(0));
#else
	return scope.Close(Undefined());
#endif
}

/*
 * Set a pin as output
 */
Handle<Value> SetOutput(const Arguments& args)
{
	HandleScope scope;
#ifdef __arm__
	if (args.Length() != 1) {
		ThrowException(Exception::TypeError(String::New("Incorrect number of arguments")));
		return scope.Close(Undefined());
	}

	if (!args[0]->IsNumber()) {
		ThrowException(Exception::TypeError(String::New("Incorrect argument type(s)")));
		return scope.Close(Undefined());
	}

	bcm2835_gpio_fsel(args[0]->NumberValue(), BCM2835_GPIO_FSEL_OUTP);

	return scope.Close(Integer::New(0));
#else
	return scope.Close(Undefined());
#endif
}

/*
 * Read from a pin
 */
Handle<Value> Read(const Arguments& args)
{
	HandleScope scope;
#ifdef __arm__
	uint8_t value;

	if (args.Length() != 1) {
		ThrowException(Exception::TypeError(String::New("Incorrect number of arguments")));
		return scope.Close(Undefined());
	}

	if (!args[0]->IsNumber()) {
		ThrowException(Exception::TypeError(String::New("Incorrect argument type(s)")));
		return scope.Close(Undefined());
	}

	value = bcm2835_gpio_lev(args[0]->NumberValue());

	return scope.Close(Integer::New(value));
#else
	return scope.Close(Undefined());
#endif
}

/*
 * Write to a pin
 */
Handle<Value> Write(const Arguments& args)
{
	HandleScope scope;
#ifdef __arm__
	if (args.Length() != 2) {
		ThrowException(Exception::TypeError(String::New("Incorrect number of arguments")));
		return scope.Close(Undefined());
	}

	if (!args[0]->IsNumber() || !args[1]->IsNumber()) {
		ThrowException(Exception::TypeError(String::New("Incorrect argument type(s)")));
		return scope.Close(Undefined());
	}

	bcm2835_gpio_write(args[0]->NumberValue(), args[1]->NumberValue());

	return scope.Close(Integer::New(0));
#else
	return scope.Close(Undefined());
#endif
}

/*
 * SPI data mode
 */
Handle<Value> spiDataMode(const Arguments& args)
{
	HandleScope scope;

#ifdef __arm__
	if (args.Length() != 1) {
		ThrowException(Exception::TypeError(String::New("Incorrect number of arguments")));
		return scope.Close(Undefined());
	}

	if (!args[0]->IsInt32()) {
		ThrowException(Exception::TypeError(String::New("Incorrect argument type(s)")));
		return scope.Close(Undefined());
	}

	bcm2835_spi_setDataMode((uint8_t) args[0]->ToInteger()->Value());

	return scope.Close(Integer::New(0));
#else
	return scope.Close(Undefined());
#endif
}

/*
 * SPI bi-directional transfer
 */
Handle<Value> spiTransfer(const Arguments& args) {
	HandleScope scope;
#ifdef __arm__
	if (args.Length() < 2) {
    ThrowException(Exception::TypeError(String::New("Incorrect number of arguments")));
    return scope.Close(Undefined());
  }

  if (!args[0]->IsObject() || !args[1]->IsInt32()) {
    ThrowException(Exception::TypeError(String::New("Incorrect argument type(s)")));
    return scope.Close(Undefined());
  }
   
  uint32_t readcount = args[1]->ToUint32()->Value();
  uint8_t writedata[readcount];
  uint8_t readdata[readcount];
  char* buf;

  if (args[0]->IsObject()) {
    Local<Object> writebuf = args[0]->ToObject();
    buf = node::Buffer::Data(writebuf);
    memcpy(writedata, buf, readcount);
  } else {
    return scope.Close(Undefined());
  }

  if (!args[2]->IsUndefined()) {
  	Local<Array> pins = Array::Cast(*args[3]);
		int len = pins->Length();
		for (int i = 0; i < len; i++) {
			Local<Object> one = pins->Get(Integer::New(i))->ToObject();
			uint8_t pin = (uint8_t)one->GetRealNamedProperty (String::New("pin"))->ToInteger()->Value();
			uint8_t val = (uint8_t)one->GetRealNamedProperty (String::New("value"))->ToInteger()->Value();
			bcm2835_gpio_write(pin, val);
		}
  }  
  bcm2835_gpio_write(args[2]->ToInteger()->Value(), 0);
  bcm2835_spi_begin();
  bcm2835_spi_transfernb((char*)writedata, (char*)readdata, readcount);
  bcm2835_spi_end();

	if (!args[2]->IsUndefined()) {
  	bcm2835_gpio_write(args[2]->ToInteger()->Value(), 1);
	}

  if (sizeof(readdata) > 0) {
    Local<Value> d;
    node::Buffer* b = node::Buffer::New((char*)readdata, readcount);     
    Local<Object> globalObj = Context::GetCurrent()->Global();
    Local<Function> bufferConstructor = Local<Function>::Cast(globalObj->Get(String::New("Buffer")));  
    Handle<Value> v[] = {b->handle_, Integer::New(readcount), Integer::New(0)};
    d = bufferConstructor->NewInstance(3, v);
    return scope.Close(d);
  } else {
		return scope.Close(Undefined());
  } 
#else
	return scope.Close(Undefined());
#endif
}

/*
 * SPI bit order
 */
Handle<Value> spiBitOrder(const Arguments& args)
{
	HandleScope scope;
#ifdef __arm__
	if (args.Length() != 1) {
		ThrowException(Exception::TypeError(String::New("Incorrect number of arguments")));
		return scope.Close(Undefined());
	}

	if (!args[0]->IsInt32()) {
		ThrowException(Exception::TypeError(String::New("Incorrect argument type(s)")));
		return scope.Close(Undefined());
	}

	bcm2835_spi_setBitOrder((uint8_t) args[0]->ToInteger()->Value());

	return scope.Close(Integer::New(0));
#else
	return scope.Close(Undefined());
#endif
}

/*
 * SPI clock divider selection
 */
Handle<Value> spiClockSpeedDivider(const Arguments& args)
{
	HandleScope scope;
#ifdef __arm__
	if (args.Length() != 1) {
		ThrowException(Exception::TypeError(String::New("Incorrect number of arguments")));
		return scope.Close(Undefined());
	}

	if (!args[0]->IsInt32()) {
		ThrowException(Exception::TypeError(String::New("Incorrect argument type(s)")));
		return scope.Close(Undefined());
	}

	bcm2835_spi_setClockDivider((uint16_t) args[0]->ToInteger()->Value());

	return scope.Close(Integer::New(0));
#else
	return scope.Close(Undefined());
#endif
}

/*
 * Initialize the bcm2835 interface and check we have permission to access it.
 */
Handle<Value> Start(const Arguments& args)
{
	HandleScope scope;
#ifdef __arm__
	if (geteuid() != 0) {
		ThrowException(Exception::Error(String::New("You must be root to access GPIO")));
		return scope.Close(Undefined());
	}

	if (!bcm2835_init()) {
		ThrowException(Exception::Error(String::New("Could not initialize GPIO")));
		return scope.Close(Undefined());
	}

  bcm2835_spi_chipSelect(BCM2835_SPI_CS_NONE);
	return scope.Close(Undefined());
#else
	return scope.Close(Undefined());
#endif
}

void init(Handle<Object> target)
{

	target->Set(String::NewSymbol("start"),
	    FunctionTemplate::New(Start)->GetFunction());

	target->Set(String::NewSymbol("setInput"),
	    FunctionTemplate::New(SetInput)->GetFunction());

	target->Set(String::NewSymbol("setOutput"),
	    FunctionTemplate::New(SetOutput)->GetFunction());

	target->Set(String::NewSymbol("read"),
	    FunctionTemplate::New(Read)->GetFunction());

	target->Set(String::NewSymbol("write"),
	    FunctionTemplate::New(Write)->GetFunction());

	target->Set(String::NewSymbol("spiDataMode"),
	    FunctionTemplate::New(spiDataMode)->GetFunction());

	target->Set(String::NewSymbol("spiTransfer"),
	    FunctionTemplate::New(spiTransfer)->GetFunction());

	target->Set(String::NewSymbol("spiBitOrder"),
	    FunctionTemplate::New(spiBitOrder)->GetFunction());

	target->Set(String::NewSymbol("spiClockSpeedDivider"),
	    FunctionTemplate::New(spiClockSpeedDivider)->GetFunction());
}

NODE_MODULE(rpio, init)
