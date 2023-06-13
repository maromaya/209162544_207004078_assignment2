from setuptools import setup, Extension

"""
A minimalist setup is shown.
"""


setup(name='mykmeanssp',
      version='1.0',
      description='kmeans in c',
      ext_modules=[Extension('mykmeanssp', sources=['kmeansmodule.c'])])
