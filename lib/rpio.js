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

var binding = require('../build/Release/rpio')
var util = require('util')
var EventEmitter = require('events').EventEmitter

function rpio() {
  EventEmitter.call(this)
}
util.inherits(rpio, EventEmitter)

/*
 * Map RPi to BCM numbering
 */
var pins = {
  1: null,
  2: null,
  3: 0,
  4: null,
  5: 1,
  6: null,
  7: 4,
  8: 14,
  9: null,
  10: 15,
  11: 17,
  12: 18,
  13: 27,
  14: null,
  15: 22,
  16: 23,
  17: null,
  18: 24,
  19: 10,
  20: null,
  21: 9,
  22: 25,
  23: 11,
  24: 8,
  25: null,
  26: 7,
  // Model B+ pins
  // 27: ID_SD
  // 28: ID_SC
  29: 5,
  // 30: ground
  31: 6,
  32: 12,
  33: 13,
  // 34: ground
  35: 19,
  36: 16,
  37: 26,
  38: 20,
  // 39: ground
  40: 21
}

function getPin(pin) {
  if (!pins[pin])
    throw "Invalid pin"
  return pins[pin]
}

binding.start()

rpio.prototype.getPin = getPin;

rpio.prototype.setInput = function(pin, cb) {
  if (binding.setInput(getPin(pin)) !== 0)
    throw "Unable to configure pin for input"
  if (cb) return cb()
}

rpio.prototype.setOutput = function(pin, cb) {
  if (binding.setOutput(getPin(pin)) !== 0)
    throw "Unable to configure pin for output"
  if (cb) return cb()
}

rpio.prototype.read = function(pin) {
  return binding.read(getPin(pin))
}

rpio.prototype.write = function(pin, value) {
  binding.write(getPin(pin), parseInt(value))
}

rpio.prototype.spiTransfer = function(data, length, cs, csmux) {
  return binding.spiTransfer(data, length, cs, csmux);
}

rpio.prototype.spiBitOrder = function(bitOrder) {
  binding.spiBitOrder(bitOrder);
}

rpio.prototype.spiDataMode = function(dataMode) {
  binding.spiDataMode(dataMode);
}

rpio.prototype.spiClockSpeedDivider = function(divider) {
  binding.spiClockSpeedDivider(divider);
}

rpio.prototype.LOW = 0x0
rpio.prototype.HIGH = 0x1

module.exports = new rpio;
