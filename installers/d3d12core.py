import os
import sys
from package_helpers import download_file, extract_zip, append_paths

def main():
    module_destination = sys.argv[1]
    if not module_destination:
        print("Invalid module destination")
        return

    major = "1"
    minor = "616"
    patch = "1"

    version_str = f"{major}.{minor}.{patch}"
    url = f"https://www.nuget.org/api/v2/package/Microsoft.Direct3D.D3D12/{version_str}"

    # Save version to file
    os.makedirs(module_destination, exist_ok=True)
    with open(append_paths(module_destination, "d3d12sdk_version.txt"), "w", encoding="utf-8") as f:
        f.write(minor)

    # Download ZIP
    zip_path = append_paths(module_destination, "d3d12core.zip")
    download_file(url, zip_path)

    # Extract ZIP
    extract_dir = append_paths(module_destination, "D3D12Core")
    extract_zip(zip_path, extract_dir)

if __name__ == "__main__":
    main()