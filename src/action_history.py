"""Append action history to a single session log file (matches terminal output)."""
from __future__ import annotations

import os
from datetime import datetime
from pathlib import Path
from typing import List, Optional, Protocol


class ActionLike(Protocol):
    timestamp: float
    type: object
    details: str


class ActionHistoryLogger:
    """One log file per run; each action is appended as it happens."""

    def __init__(self, log_dir: str | Path = "logs") -> None:
        self.log_dir = Path(log_dir)
        self.path: Optional[Path] = None
        self.face_name: Optional[str] = None
        self._announced = False

    def _ensure_path(self, face_name: str) -> Path:
        if self.path is None:
            self.log_dir.mkdir(parents=True, exist_ok=True)
            stamp = datetime.now().strftime("%Y%m%d%H%M%S")
            self.face_name = face_name
            self.path = self.log_dir / f"{face_name}_history_{stamp}.txt"
            self._announced = False
        return self.path

    def log_action(self, face_name: str, action: ActionLike) -> None:
        path = self._ensure_path(face_name)
        if not self._announced:
            print(f"[ActionHistory] Logging to {path}")
            self._announced = True
        time_str = datetime.fromtimestamp(action.timestamp).strftime("%Y-%m-%d %H:%M:%S.%f")
        line = f"{time_str} - {action.type.name}: {action.details}\n"
        with open(path, "a", encoding="utf-8") as f:
            f.write(line)

    def log_actions(self, face_name: str, actions: List[ActionLike]) -> None:
        for action in actions:
            self.log_action(face_name, action)


def save_action_history(face_name: str, actions: List[ActionLike], logger: Optional[ActionHistoryLogger] = None) -> None:
    """Backward-compatible batch save; prefers live logger when provided."""
    if not actions:
        return
    if logger is not None:
        logger.log_actions(face_name, actions)
        return
    os.makedirs("logs", exist_ok=True)
    timestamp = datetime.now().strftime("%Y%m%d%H%M%S")
    filename = f"{face_name}_history_{timestamp}.txt"
    with open(os.path.join("logs", filename), "w", encoding="utf-8") as f:
        for action in actions:
            time_str = datetime.fromtimestamp(action.timestamp).strftime("%Y-%m-%d %H:%M:%S.%f")
            f.write(f"{time_str} - {action.type.name}: {action.details}\n")
