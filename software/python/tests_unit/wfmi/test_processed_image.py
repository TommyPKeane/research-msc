import logging
import pathlib

import PIL
import pytest

from wfmi.processed_image import (
    opencv_image_to_pillow_image,
    pillow_image_to_opencv_image,
    processed_images_are_similar,
    ProcessedImage,
)


module_log = logging.getLogger(__name__)


@pytest.fixture(scope="session")
def image_path_left() -> pathlib.Path:
    return pathlib.Path("../../thesis/images/AGS2L001.jpg")


@pytest.fixture(scope="session")
def image_path_right() -> pathlib.Path:
    return pathlib.Path("../../thesis/images/AGS2R001.jpg")


@pytest.fixture(scope="session")
def image_obj_left(image_path_left) -> PIL.Image.Image:
    return PIL.Image.open(image_path_left)


@pytest.fixture(scope="session")
def image_obj_right(image_path_right) -> PIL.Image.Image:
    return PIL.Image.open(image_path_right)


@pytest.fixture(scope="session")
def expectedly_similar() -> bool:
    return True


@pytest.mark.parametrize(
    "image_path, image_name",
    (
        ("image_path_left", "image_path_left"),
        ("image_path_right", "image_path_right"),
    )
)
def test_ProcessedImage_from_file(request, image_path, image_name) -> None:
    image_path = request.getfixturevalue(image_path)
    image_name = str(request.getfixturevalue(image_name).resolve())
    processed_img = ProcessedImage.from_file(image_path)
    module_log.debug(f"ProcessedImage Name: {processed_img.name}")
    module_log.debug(f"image_name: {image_name}")
    assert processed_img.name == image_name
    return None


@pytest.mark.parametrize(
    "image_obj, image_name",
    (
        ("image_obj_left", "image_path_left"),
        ("image_obj_right", "image_path_right"),
    )
)
def test_ProcessedImage_from_image(request, image_obj, image_name) -> None:
    image_obj = request.getfixturevalue(image_obj)
    image_name = str(request.getfixturevalue(image_name).resolve())
    processed_img = ProcessedImage.from_image(image_obj)
    module_log.debug(f"ProcessedImage Name: {processed_img.name}")
    module_log.debug(f"image_name: {image_name}")
    assert processed_img.name == image_name
    return None


def test_processed_images_are_similar(image_obj_left, image_obj_right, expectedly_similar):
    processed_img_left = ProcessedImage.from_image(image_obj_left)
    processed_img_right = ProcessedImage.from_image(image_obj_right)
    are_similar = processed_images_are_similar(
        processed_img_left,
        processed_img_right,
    )
    assert are_similar == expectedly_similar
    return None
