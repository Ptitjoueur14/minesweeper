{
  stdenv,

  pkg-config,
  sdl3,
  SDL2,
  SDL2_image,
  SDL2_ttf,
}:

stdenv.mkDerivation {
  name = "minesweeper";

  src = ./.;

  nativeBuildInputs = [
    pkg-config
  ];

  buildInputs = [
    sdl3
    SDL2
    SDL2_image
    SDL2_ttf
  ];

  installPhase = ''
    mkdir -p $out/bin
    cp minesweeper $out/bin
  '';

  meta = {
    mainProgram = "minesweeper";
  };
}
