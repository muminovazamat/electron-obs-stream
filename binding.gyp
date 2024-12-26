{
  "targets": [
    {
      "target_name": "obsaddon",
      "sources": [ "obsaddon.cpp" ],
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "include_dirs": [
        "<!(node -p \"require('node-addon-api').include\")",
        "/usr/include/obs"
      ],
      'libraries': [
          "-L/usr/lib/x86_64-linux-gnu/",
          "-lobs",
          "-lobs-frontend-api"
      ]
    }
  ]
}