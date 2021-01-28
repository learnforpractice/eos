import sys

from skbuild import setup

# Require pytest-runner only when running tests
pytest_runner = (['pytest-runner>=2.0,<3dev']
                 if any(arg in sys.argv for arg in ('pytest', 'test'))
                 else [])

setup_requires = pytest_runner

setup(
    name="uuosio",
    version="1.2.4",
    description="uuosio project",
    author='The scikit-build team',
    license="MIT",
    packages=['src'],
    package_dir={'uuosio': 'src/'},
    install_requires=['cython'],
    tests_require=['pytest'],
    setup_requires=setup_requires
)
