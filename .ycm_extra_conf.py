def Settings( **kwargs ):
  return {
    'flags': [ '-Icomponents/libopencm3/include', '-I./', '-Isrc', '-DSTM32L0'],
  }
