{
  description = "Tablo Udp Discovery";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
  };

  outputs =
    { self, nixpkgs }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs { inherit system; };

      version = "0.3";

      commonDeps = with pkgs; [
        cmake
        gcc
        gnumake
        libtasn1
      ];

      mkTUDPackage =
        {
          pname,
          buildTarget,
          enableLib ? false,
          enableClient ? false,
          enableServer ? false,
          extraInputs ? [ ],
        }:
        pkgs.stdenv.mkDerivation {
          inherit pname version;
          src = ./.;

          buildInputs = commonDeps ++ extraInputs;

          configurePhase = ''
            cmake -B build -S $src \
              -DCMAKE_BUILD_TYPE=Release \
              -DDEF_TUD=${if enableLib then "ON" else "OFF"} \
              -DDEF_CLIENT=${if enableClient then "ON" else "OFF"} \
              -DDEF_SERVER=${if enableServer then "ON" else "OFF"}
          '';

          buildPhase = ''
            cmake --build build \
              --target ${buildTarget} \
              -j$NIX_BUILD_CORES
          '';

          installPhase = ''
            cmake --install build --prefix=$out
            cp LICENSE $out/
          '';
        };

    in
    {
      packages.${system} =
        let
          lib = mkTUDPackage {
            pname = "libtud";
            buildTarget = "tud";
            enableLib = true;
          };
        in
        {
          inherit lib;

          client = mkTUDPackage {
            pname = "tud-client";
            buildTarget = "tud-client";
            enableClient = true;
            extraInputs = [ lib ];
          };

          server = mkTUDPackage {
            pname = "tud-server";
            buildTarget = "tud-server";
            enableServer = true;
            extraInputs = [ lib ];
          };

          full = mkTUDPackage {
            pname = "libtud-full";
            buildTarget = "all";
            enableLib = true;
            enableServer = true;
            enableClient = true;
          };

          default = self.packages.${system}.lib;
        };

      devShells.${system}.default = pkgs.mkShell {
        packages = commonDeps ++ [
          pkgs.bridge-utils
          pkgs.clang-tools
        ];

        shellHook = ''
          git status
        '';
      };
    };
}
