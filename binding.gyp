{
  "targets": [
    {
      "target_name": "rpio",
      'cflags_cc': [
        '-fpermissive',
        '-D'
      ],
      "sources": [
        "src/bcm2835.cc",
        "src/rpio.cc"
      ]
    }
  ]
}
