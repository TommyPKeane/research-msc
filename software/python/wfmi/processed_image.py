import logging
import math
import pathlib
import typing

import cv2
import numpy
import PIL.Image
import PIL.ImageFilter


module_log: logging.Logger = logging.getLogger(__name__)


# https://pillow.readthedocs.io/en/stable/handbook/concepts.html#concept-modes
PIL_MODES: dict = {
    "no_color": (
        "1",
        "L",
        "I",
        "F",
        "La",  # Premultiplied Transparency
        "I;16",
        "I;16L",
        "I;16B",
        "I;16N",
    ),
    "color": (
        "P",
        "RGB",
        "RGBX",
        "RGBa",  # Premultiplied Transparency
        "CMYK",
        "YCbCr",
        "LAB",
        "HSV",
        "BGR;15",
        "BGR;16",
        "BGR;24",
        "BGR;32",
    ),
    "no_color_transparent": (
        "LA",
    ),
    "color_transparent": (
        "RGBA",
        "PA",
    ),
}

PIL_MODES_1CHANNEL = (
    "1",
    "L",
    "I",
    "F",
    "La",  # Premultiplied Transparency
    "I;16",
    "I;16L",
    "I;16B",
    "I;16N",
)
PIL_MODES_2CHANNEL = (
    "LA",
)
PIL_MODES_3CHANNEL = (
    "RGB",
    "RGBX",
    "RGBa",  # Premultiplied Transparency
    "LAB",
    "HSV",
    "BGR;15",
    "BGR;16",
    "BGR;24",
    "BGR;32",
)
PIL_MODES_4CHANNEL = (
    "RGBA",
    "CMYK",
    "YCbCr",
)


def pillow_image_to_opencv_image(pillow_image: PIL.Image.Image) -> numpy.ndarray:
    pillow_data = numpy.array(pillow_image)
    opencv_image = cv2.cvtColor(pillow_data, cv2.cv.CV_RGB2BGR)
    return opencv_image


def opencv_image_to_pillow_image(opencv_image: numpy.array) -> PIL.Image.Image:
    opencv_image_rgb = cv2.cvtColor(opencv_image, cv2.cv.CV_BGR2RGB)
    pillow_image = PIL.Image.fromarray(opencv_image_rgb)
    return opencv_image


class ProcessedImage():
    _image: PIL.Image.Image = None
    _preprocessed_image: PIL.Image.Image = None
    _postprocessed_image: PIL.Image.Image = None
    _name: str = None
    _size: tuple[int] = None
    _rows: int = None
    _cols: int = None
    _channels: int = None
    _cols: int = None
    _has_color: bool = None
    _has_transparency: bool = None

    def __init__(self, image_obj: PIL.Image.Image) -> None:
        self._image = image_obj
        self._name = (self._image.filename if self._image.filename else "unknown")
        self._parse_image()
        return None

    @classmethod
    def from_file(cls: object, image_path: pathlib.Path, *args, **kw_args) -> "ProcessedImage":
        image_obj: PIL.Image.Image = PIL.Image.open(image_path, *args, **kw_args)
        return cls(image_obj)

    @classmethod
    def from_image(cls: object, image_obj: PIL.Image.Image) -> "ProcessedImage":
        return cls(image_obj)

    def _parse_image(self) -> None:
        self._size: tuple[int] = self._image.size  # (width, height) = (cols, rows)
        self._rows: int = self._image.height
        self._cols: int = self._image.width

        image_mode: str = self._image.mode

        if (image_mode in PIL_MODES["no_color"]):
            self._has_color: bool = False
            self._has_transparency: bool = False
        elif (image_mode in PIL_MODES["no_color_transparent"]):
            self._has_color: bool = False
            self._has_transparency: bool = True
        elif (image_mode in PIL_MODES["color"]):
            self._has_color: bool = True
            self._has_transparency: bool = False
        elif (image_mode in PIL_MODES["color_transparent"]):
            self._has_color: bool = True
            self._has_transparency: bool = True
        else:
            self._has_color: bool = None
            self._has_transparency: bool = None

        if (image_mode in PIL_MODES_1CHANNEL):
            self._channels: int = 1
        elif (image_mode in PIL_MODES_2CHANNEL):
            self._channels: int = 2
        elif (image_mode in PIL_MODES_3CHANNEL):
            self._channels: int = 3
        elif (image_mode in PIL_MODES_4CHANNEL):
            self._channels: int = 4
        else:
            self._channels: int = None

        return None

    @property
    def name(self) -> typing.Optional[str]:
        return self._name

    @name.setter
    def name(self, new_name: str) -> None:
        self._name = new_name
        return None

    @property
    def num_channels(self) -> typing.Optional[int]:
        return self._channels

    def get_channel_means(self) -> dict:
        channel_means = {}
        channel_labels = tuple(self._image.mode)
        for i in range(self.num_channels):
            channel_data = tuple(self._image.getdata(band=i))
            channel_means[channel_labels[i]] = math.fsum(channel_data) / len(channel_data)
        return channel_means

    def preprocess_image(
        self,
        needs_preprocessing: bool = True,
        blur_radius: int = 3,
    ) -> None:
        """Summary

        Args:
            needs_preprocessing: ...
            blur_radius: ...

        Returns:
            None: Nothing to Return, Updates the `self._preprocessed_image`

        See Also:
        - `self.postprocess_image()`
        - `processed_images_are_similar()`
        """
        if needs_preprocessing:
            blurred_image = self._image.filter(
                PIL.ImageFilter.GaussianBlur(radius=blur_radius),
            )
            blurred_cvimage = pillow_image_to_opencv_image(blurred_image)
            laplacian_kernel = numpy.array(
                [
                  [-1, -1, -1],
                  [-1,  8, -1],
                  [-1, -1, -1],
                ]
            )
            blurred_cvimage_float = cv2.normalize(
                filtered_cvimage,
                None,
                0.0,
                1.0,
                cv2.NORM_MINMAX,
                dtype=cv2.CV_64FC3,
            )
            filtered_cvimage = cv2.filter2D(
                blurred_cvimage_float,
                -1,
                laplacian_kernel,
            )
            normalized_cvimage = cv2.normalize(
                filtered_cvimage,
                None,
                0,
                255,
                cv2.NORM_MINMAX,
                dtype=cv2.CV_8UC3,
            )
            self._preprocessed_image = opencv_image_to_pillow_image(normalized_cvimage)
            module_log.debug(f"Pre-Processed Image: {self._name}")
        else:
            module_log.debug(f"Not Pre-Processing Image: {self._name}")
        return None

    def postprocess_image(
        self,
        needs_postprocessing: bool = True,
        channel_offsets: tuple[float] = None,
    ) -> None:
        """Summary

        Args:
            needs_postprocessing: ...
            channel_offsets: ...

        Returns:
            None: Nothing to Return, Updates the `self._postprocessed_image`

        See Also:
        - `self.preprocess_image()`
        - `processed_images_are_similar()`
        """
        if needs_postprocessing:
            if channel_offsets:
                _preprocessed_image_f64 = cv2.normalize(
                    self._preprocessed_image,
                    None,
                    0.0,
                    1.0,
                    cv2.NORM_MINMAX,
                    dtype=cv2.CV_64FC3,
                )
                adjusted_cvimage = None  # TODO (tommypkeane): Implement offset adjustment
                self._postprocessed_image = opencv_image_to_pillow_image(adjusted_cvimage)
                module_log.debug(f"Pre-Processed Image: {self._name}")
            else:
                raise ValueError(
                    f"Postprocessing Requires Channel Offsets | channel_offsets: {channel_offsets}"
                )
        else:
            module_log.debug(f"Not Pre-Processing Image: {self._name}")
        return None


def processed_images_are_similar(
    image_a: ProcessedImage,
    image_b: ProcessedImage,
    r_channel_diff_max: int = 10,
    g_channel_diff_max: int = 5,
    b_channel_diff_max: int = 10,
    a_channel_diff_max: int = 10,
    l_channel_diff_max: int = 10,
    avg_channel_diff_max: int = 10,
) -> bool:
    """Custom Validation to Compare the Channel Means of Images to see if they Need Pre-Processing

    This is a simple check to see if the results are visually different, which
    can cause the gradients to be different and the registration to fail. A more
    complex method could be put in place, but given research constraints an
    automatic method was not known at the time, without much more extensive
    research.

    Currently color difference is the only measure, even though the images are
    also expected to be sharpened if the result here is `False`.

    Again, an automatic and reliably safe method for accurately determining
    image "sharpenness" was not known without more extensive research. And
    sharpening of low entropy (repeated texture) images can actually sway the
    registration algorithm to fail erroneously.

    For example, tiled floors can match in arbitrary orientations, especially if
    overly sharpened to make their pattern have a higher gradient in the pixel
    intensity/-ies.
    """
    similar = None

    channel_means_a: dict = image_a.get_channel_means()
    channel_means_b: dict = image_b.get_channel_means()

    module_log.debug(f"image_a Channel Means: {channel_means_a}")
    module_log.debug(f"image_b Channel Means: {channel_means_b}")

    if (image_a.num_channels == image_b.num_channels):
        if image_a.num_channels == 1:
            similar = math.isclose(
                tuple(channel_means_a.values())[0],
                tuple(channel_means_b.values())[0],
                abs_tol=l_channel_diff_max,
            )
        elif image_a.num_channels == 3:
            r_similar = math.isclose(
                channel_means_a.get("R", 0.0),
                channel_means_b.get("R", 0.0),
                abs_tol=r_channel_diff_max,
            )
            g_similar = math.isclose(
                channel_means_a.get("G", 0.0),
                channel_means_b.get("G", 0.0),
                abs_tol=g_channel_diff_max,
            )
            b_similar = math.isclose(
                channel_means_a.get("B", 0.0),
                channel_means_b.get("B", 0.0),
                abs_tol=b_channel_diff_max,
            )
            similar = (r_similar and g_similar and b_similar)
        else:
            raise NotImplemented(f"Code Doesn't Yet Support these Image Formats | {image_a.num_channels} Channels")
    else:
        module_log.warning(
            f"Images have Different Numbers of Channels | A: {image_a.num_channels} | B: {image_b.num_channels}"
        )
        channel_means_a_values = channel_means_a.values()
        channel_means_b_values = channel_means_b.values()
        channel_means_a_mean = math.fsum(channel_means_a_values) / len(channel_means_a_values)
        channel_means_b_mean = math.fsum(channel_means_b_values) / len(channel_means_b_values)
        channel_means_abs_diff = math.fabs()
        similar = math.isclose(
            channel_means_a_mean,
            channel_means_b_mean,
            abs_tol=avg_channel_diff_max,
        )

    return similar
