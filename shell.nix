{ config ? {}, overlays ? [], pkgs ? import ./contrib/nix { inherit config overlays; } }:

pkgs.mkShell {
  buildInputs = with pkgs; [
    arduino-cli
    platformio
    reuse
  ];
}
