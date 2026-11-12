{
  inputs = {
    nixpkgs.url = "https://channels.nixos.org/nixos-unstable/nixexprs.tar.xz";
  };

  outputs =
    { self, nixpkgs }:
    let
      inherit (nixpkgs.lib) genAttrs getExe;

      forAllSystems = genAttrs [
        "x86_64-linux"
        "aarch64-linux"
        "aarch64-darwin"
      ];
      forAllPkgs = function: forAllSystems (system: function pkgs.${system});

      mkApp = (
        program: {
          type = "app";
          inherit program;
        }
      );

      pkgs = forAllSystems (
        system:
        import nixpkgs {
          inherit system;
          overlays = [ ];
        }
      );
    in
    {
      formatter = forAllPkgs (pkgs: pkgs.nixfmt-tree);

      packages = forAllPkgs (pkgs: rec {
        default = minesweeper;
        minesweeper = pkgs.callPackage ./package.nix { };
      });
      apps = forAllSystems (system: rec {
        default = minesweeper;
        minesweeper = mkApp (getExe self.packages.${system}.minesweeper);
      });

      devShells = forAllPkgs (pkgs: {
        default =
          pkgs.mkShell.override
            {
              stdenv = pkgs.clangStdenv;
            }
            {
              packages = with pkgs; [
                bear

                pkg-config
                sdl3
                SDL2
                SDL2_image
                SDL2_ttf
              ];
            };
      });
    };
}
