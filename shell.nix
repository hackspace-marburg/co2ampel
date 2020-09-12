{ config ? {}, overlays ? [], pkgs ? import ./nix { inherit config overlays; } }:

pkgs.mkShell {
  buildInputs = with pkgs; [
    arduino-cli
    platformio
  ];
}
