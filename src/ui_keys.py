"""OpenCV window keyboard helpers (Windows-friendly quit + arrows)."""
from __future__ import annotations

from typing import Optional, Tuple

import cv2


def focus_cv_window(window_name: str) -> None:
    """Bring the OpenCV window forward once so keys are received."""
    try:
        cv2.setWindowProperty(window_name, cv2.WND_PROP_TOPMOST, 1)
        cv2.setWindowProperty(window_name, cv2.WND_PROP_TOPMOST, 0)
    except cv2.error:
        pass


def pump_cv_key(wait_ms: int = 25) -> int:
    """
    Poll keyboard input from the active OpenCV window.
    Uses a short multi-poll so slow frames do not drop quit keys on Windows.
    """
    wait_ms = max(1, int(wait_ms))
    key_raw = -1
    for delay in (wait_ms, wait_ms):
        k = cv2.waitKey(delay)
        if k >= 0:
            key_raw = k
    return key_raw


def decode_cv_key(key_raw: int) -> Tuple[Optional[int], Optional[int]]:
    """
    Split waitKey result into (ascii_key, arrow_key).
    arrow_key uses Windows extended codes: Left=75, Right=77.
    """
    if key_raw < 0:
        return None, None
    low = key_raw & 0xFF
    if low in (0, 224):
        arrow = (key_raw >> 8) & 0xFF
        return None, arrow if arrow else None
    return low, None


def is_quit_key(ascii_key: Optional[int]) -> bool:
    return ascii_key in (ord("q"), ord("Q"), 27)
