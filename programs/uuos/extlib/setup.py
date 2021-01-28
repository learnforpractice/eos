import sys

from skbuild import setup

# Require pytest-runner only when running tests
pytest_runner = (['pytest-runner>=2.0,<3dev']
                 if any(arg in sys.argv for arg in ('pytest', 'test'))
                 else [])

setup_requires = pytest_runner

setup(
    name="uuosio",
    version="0.1.0",
    description="UUOSIO project",
    author='The UUOSIO Team',
    license="MIT",
    packages=['uuosio'],
    package_dir={'uuosio': 'uuosio/'},
#    package_data={'uuosio': ['data/hello.txt', 'data/hello2.txt']},
    data_files = [('lib', ['lib/libchain_api.dylib']), ('bin', ['bin/uuos'])],
    scripts=['bin/run-uuos'],
    install_requires=['cython'],
    tests_require=['pytest'],
    setup_requires=setup_requires,
#    include_package_data=True
)
