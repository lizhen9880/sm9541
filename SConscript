from building import *
Import('rtconfig')

src   = []
cwd   = GetCurrentDir()

# add sm9541 src files.
if GetDepend('PKG_USING_SM9541'):
    src += Glob('src/sm95_device.c')

if GetDepend('PKG_USING_SM9541_SAMPLE'):
    src += Glob('sample/sm9541_sample.c')

# add sm9541 include path.
path = [cwd + '/inc']

group = DefineGroup('sm9541', src, depend = ['PKG_USING_SM9541'], CPPPATH = path)

Return('group')
