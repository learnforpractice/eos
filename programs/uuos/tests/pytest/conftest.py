import pytest
from uuos import application

logger = application.get_logger(__name__)

@pytest.fixture(scope='function', autouse=True)
def test_log(request):
    return
    logger.info("Test '{}' STARTED".format(request.node.nodeid))
    def fin():
        logger.info("Test '{}' COMPLETED".format(request.node.nodeid))
    request.addfinalizer(fin)
