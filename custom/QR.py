#!/usr/bin/env -S ${FDE}/fde-venv/bin/python3

import os
# Keep your path tweak, even though ZBar isn't used anymore
# os.environ["DYLD_LIBRARY_PATH"] = "/opt/homebrew/opt/zbar/lib:" + os.environ.get("DYLD_LIBRARY_PATH", "")
# LLM Generated Source Code

import cv2
import numpy as np
import subprocess
import sys

class QRCodeScanner:
    def __init__(self):
        self.copied = False
        self.detector = cv2.QRCodeDetector()

    def copy_to_clipboard(self, text: str):
        try:
            if sys.platform == "linux":
                command = ["xsel","-b"]
            elif sys.platform == "darwin":
                command = ["pbcopy"]
            else:
                raise PermissionError
            subprocess.run(command, input=text.encode("utf-8"), check=True)
            self.copied = True
            print("[✅] QR data copied to clipboard.", file=sys.stderr)
        except Exception as e:
            print(f"[!] Failed to copy to clipboard: {e}. Help yourself!", file=sys.stderr)
            print(text)

    def draw_polylines(self, frame, points):
        # points: Nx4x2 for multi, or 1xN?
        if points is None:
            return
        if isinstance(points, list):
            pts_list = points
        else:
            # OpenCV often returns a numpy array shape (N, 4, 2)
            pts_list = [p for p in points]

        for p in pts_list:
            if p is None:
                continue
            pts = np.array(p, dtype=np.int32)
            cv2.polylines(frame, [pts], True, (255, 0, 255), 3)

    def run(self, cam_index=0):
        cap = cv2.VideoCapture(cam_index)
        if not cap.isOpened():
            raise RuntimeError(f"Could not open webcam (index {cam_index}).")

        window = "Detect QR Code from Webcam (press q to quit)"
        cv2.namedWindow(window, cv2.WINDOW_NORMAL)

        while True:
            ok, frame = cap.read()
            if not ok:
                break

            # Try multi first (available in recent OpenCV builds)
            # retval is bool; decoded_info is list[str]; points is array/list of polygons
            retval, decoded_info, points, _ = self.detector.detectAndDecodeMulti(frame)

            found_texts = []
            if retval and decoded_info:
                # Filter out empty strings (when a box is detected but not decoded)
                found_texts = [s for s in decoded_info if s]

            # Fallback to single if multi gave nothing
            if not found_texts:
                data_single, points_single, _ = self.detector.detectAndDecode(frame)
                if data_single:
                    found_texts = [data_single]
                    # Normalize points to list-of-polygons shape for drawing
                    if points_single is not None:
                        points = [points_single]

            # Draw boxes if we have any
            if points is not None:
                self.draw_polylines(frame, points)

            # On first successful decode, copy to clipboard and exit
            if found_texts and not self.copied:
                self.copy_to_clipboard(found_texts[0])
                # Show the box briefly, then exit
                cv2.imshow(window, frame)
                cv2.waitKey(300)  # 300 ms so you can see the highlight
                break

            cv2.imshow(window, frame)

            # Quit manually
            if (cv2.waitKey(1) & 0xFF) == ord('q'):
                break

        cap.release()
        cv2.destroyAllWindows()

if __name__ == "__main__":
    QRCodeScanner().run()

