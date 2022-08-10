# YAML format

Other than the Tiled JSON and XML formats, Tactile supports its own custom YAML map format. If you're familiar with the
Tiled JSON format, you'll notice many similarities. However, one of the key differences is that the Tactile YAML format
doesn't support embedded tilesets. All predefined names are in `kebab-case`.

All attributes that aren't specified as required may be omitted from save files.

## Root

The root node in the Tactile YAML format provides general information about the tilemap.

|               Attribute |                        Type                         | Required | Description                           |
|------------------------:|:---------------------------------------------------:|:--------:|:--------------------------------------|
|               `version` |                        `int`                        |   Yes    | The version of the YAML map format.   |
|             `row-count` |                        `int`                        |   Yes    | The number of rows in the tilemap.    |
|          `column-count` |                        `int`                        |   Yes    | The number of columns in the tilemap. |
|            `tile-width` |                        `int`                        |   Yes    | The width of tiles in the tilemap.    |
|           `tile-height` |                        `int`                        |   Yes    | The height of tiles in the tilemap.   |
|         `next-layer-id` |                        `int`                        |   Yes    | The next available layer ID.          |
|        `next-object-id` |                        `int`                        |   Yes    | The next available object ID.         |
|           `tile-format` |                    `TileFormat`                     |    No    |                                       |
|                `layers` |                      Sequence                       |    No    | A sequence of `Layer` nodes.          |
|              `tilesets` |                      Sequence                       |    No    | A sequence of `TilesetRef` nodes.     |
|            `properties` |                      Sequence                       |    No    | A sequence of `Property` nodes.       |
| `component-definitions` |                      Sequence                       |    No    | A sequence of `ComponentDef` nodes.   |
|            `components` |                      Sequence                       |    No    | A sequence of `Component` nodes.      |

---

## `TileFormat`

Provides information about the representation of tile layer data.

Using a `encoding` other than `plain` leads to tile layer data being encoded as a one-dimensional array of 32-bit signed
integers. The tiles are arranged by appending each row after another, starting at the row at index zero (the top
row in the editor).

The emitter works by first compressing the tile data (unless `compression` is set to `none`), and then encoding the
possibly compressed data. Omitting `encoding` or setting it to `plain` leads to tile layer data to be simply stored as a
string of integers separated by spaces (potentially formatted with newlines).

Note, it's not possible to specify a compression strategy whilst using `plain` encoding!

The `zlib-compression-level` attribute directly corresponds to Zlib constants such as `Z_DEFAULT_COMPRESSION` (which
is `-1`), `Z_BEST_SPEED` (which is `1`), and `Z_BEST_COMPRESSION` (which is `9`). You can use other values in the
interval `[1, 9]` to request an intermediate compression mode, depending on the tradeoff you want to make.

|                Attribute |                 Type                 | Required | Default  | Description                                   |
|-------------------------:|:------------------------------------:|:--------:|:--------:|:----------------------------------------------|
|               `encoding` |       One of `plain`, `base64`       |    No    | `plain`  | Tile layer data encoding.                     |
|            `compression` |        One of `none`, `zlib`         |    No    |  `none`  | The compression algorithm used.               |
| `zlib-compression-level` | Either `-1` or in the range `[1, 9]` |    No    |   `-1`   |                                               |
|             `endianness` |        One of `little`, `big`        |    No    | `little` | Byte ordering of compressed tile identifiers. |

Example:

```YAML
encoding: base64
compression: zlib
zlib-compression-level: 9
```

---

## `Layer`

TODO v2: consider removing `id`

|    Attribute |                         Type                         | Required | Default | Description                                      |
|-------------:|:----------------------------------------------------:|:--------:|:-------:|:-------------------------------------------------|
|       `name` |                       `string`                       |   Yes    |   N/A   | The name associated with the layer.              |
|         `id` |                        `int`                         |   Yes    |   N/A   | The unique identifier associated with the layer. |
|       `type` | One of `tile-layer`, `object-layer` or `group-layer` |   Yes    |   N/A   |                                                  |
|    `opacity` |                       `float`                        |    No    |  `1.0`  | The opacity of the layer, in the range [0, 1].   |
|    `visible` |                        `bool`                        |    No    | `true`  | Whether or not the layer is rendered.            |
| `properties` |                       Sequence                       |    No    |   N/A   | A sequence of `Property` nodes.                  |

Example:

```YAML
name: foo
id: 7
type: tile-layer
data: |
  1 2 3 4
  5 6 7 8
```

In addition to the layer attributes listed above, each of the three different kinds of layers have additional
attributes.

### `TileLayer`

| Attribute |   Type   | Required | Description                                                   |
|----------:|:--------:|:--------:|:--------------------------------------------------------------|
|    `data` | `string` |   Yes    | A string of (potentially compressed) global tile identifiers. |

### `ObjectLayer`

| Attribute |   Type   | Required | Description                   |
|----------:|:--------:|:--------:|:------------------------------|
| `objects` | Sequence |   Yes    | A sequence of `Object` nodes. |

### `GroupLayer`

| Attribute |   Type   | Required | Description                  |
|----------:|:--------:|:--------:|:-----------------------------|
|  `layers` | Sequence |   Yes    | A sequence of `Layer` nodes. |

---

## `Object`

|    Attribute |                Type                 | Required | Default | Description                     |
|-------------:|:-----------------------------------:|:--------:|:-------:|:--------------------------------|
|         `id` |                `int`                |   Yes    |   N/A   |                                 |
|       `type` | One of `point`, `rect` or `ellipse` |   Yes    |   N/A   |                                 |
|       `name` |              `string`               |    No    |  `""`   |                                 |
|        `tag` |              `string`               |    No    |  `""`   | A user-defined type tag.        |
|    `visible` |               `bool`                |    No    | `true`  |                                 |
|          `x` |               `float`               |    No    |   `0`   |                                 |
|          `y` |               `float`               |    No    |   `0`   |                                 |
|      `width` |               `float`               |    No    |   `0`   |                                 |
|     `height` |               `float`               |    No    |   `0`   |                                 |
| `properties` |              Sequence               |    No    |   N/A   | A sequence of `Property` nodes. |

Example:

```YAML
name: foo
id: 42
type: point
tag: spawn-point
x: 123
y: 456
```

---

## `TilesetRef`

The `TilesetRef` node provides information about an external tileset definition, and are always stored in the main
tilemap file.

TODO v2: change `first-global-id` to `first-tile`.

|         Attribute |   Type   | Required | Description                                           |
|------------------:|:--------:|:--------:|:------------------------------------------------------|
| `first-global-id` |  `int`   |   Yes    | The first global tile ID associated with the tileset. |
|            `path` | `string` |   Yes    | Relative path to the tileset definition file.         |

Example:

```YAML
first-global-id: 42
path: foo/bar/tileset.yaml
```

---

## `Tileset`

The `Tileset` node is always defined in an external YAML file, since embedded tilesets are not supported by the Tactile
YAML format. By default, a tileset that is added to a map in the Tactile editor will be stored next to the main map
file. Note, the `tiles` node only contains tiles that feature additional data, such as properties and animations.

|      Attribute |   Type   | Required | Description                                |
|---------------:|:--------:|:--------:|:-------------------------------------------|
|      `version` |  `int`   |   Yes    |                                            |
|         `name` | `string` |   Yes    | The name of the tileset.                   |
|   `tile-count` |  `int`   |   Yes    | The total amount of tiles in the tileset.  |
|   `tile-width` |  `int`   |   Yes    | The width of each tile in the tileset.     |
|  `tile-height` |  `int`   |   Yes    | The height of each tile in the tileset.    |
| `column-count` |  `int`   |   Yes    | The amount of tile columns in the tileset. |
|   `image-path` | `string` |   Yes    | Relative path to the tileset image.        |
|  `image-width` |  `int`   |   Yes    | The width of the tileset image.            |
| `image-height` |  `int`   |   Yes    | The height of the tileset image.           |
|        `tiles` | Sequence |    No    | A sequence of `Tile` nodes.                |
|   `properties` | Sequence |    No    | A sequence of `Property` nodes.            |

Example:

```YAML
version: 1
name: foo
tile-count: 1024
tile-width: 32
tile-height: 32
column-count: 32
image-path: ../foo.png
image-width: 1024
image-height: 1024
```

### `Tile`

The `Tile` node provides additional information about tiles in a tileset.

TODO v2: change `id` to `index`.

|    Attribute |   Type   | Required | Description                      |
|-------------:|:--------:|:--------:|:---------------------------------|
|         `id` |  `int`   |   Yes    | Local ID of the associated tile. |
|  `animation` | Sequence |    No    | A sequence of `Frame` nodes.     |
|    `objects` | Sequence |    No    | A sequence of `Object` nodes.    |
| `properties` | Sequence |    No    | A sequence of `Property` nodes.  |

Example:

```YAML
id: 42
animation:
  - tile: 123
    duration: 150
  - tile: 456
    duration: 120
```

### `Frame`

The `Frame` node represents a frame in an animation.

TODO v2: change `tile` to `tile-index`.

|  Attribute | Type  | Required | Description                                                    |
|-----------:|:-----:|:--------:|:---------------------------------------------------------------|
|     `tile` | `int` |   Yes    | Local ID of the tile that should be rendered during the frame. |
| `duration` | `int` |   Yes    | Duration of the frame, in milliseconds.                        |

---

## `ComponentDef`

|    Attribute |   Type   | Required | Description                                 |
|-------------:|:--------:|:--------:|:--------------------------------------------|
|       `name` | `string` |   Yes    | The name of the component.                  |
| `attributes` | Sequence |    No    | A sequence of `ComponentDefAttribute` nodes |

## `ComponentDefAttribute`

| Attribute |                                 Type                                 | Required | Description                  |
|----------:|:--------------------------------------------------------------------:|:--------:|:-----------------------------|
|    `name` |                               `string`                               |   Yes    | The name of the attribute.   |
|    `type` | One of `string`, `int`, `float`, `bool`, `color`, `file` or `object` |   Yes    |                              |
| `default` |                              `variant`                               |    No    | The default attribute value. |

Example:

```YAML
name: GameObject
attributes:
  - name: opacity
    type: float
    default: 1.0
  - name: health
    type: int
    default: 100
  - name: tag
    type: string
```

---

## `Component`

| Attribute |   Type   | Required | Description                                |
|----------:|:--------:|:--------:|:-------------------------------------------|
|    `type` | `string` |   Yes    | The name of the component definition type. |
|  `values` | Sequence |    No    | A sequence of `ComponentAttribute` nodes.  |

## `ComponentAttribute`

| Attribute |   Type    | Required | Description                |
|----------:|:---------:|:--------:|:---------------------------|
|    `name` | `string`  |   Yes    | The name of the attribute. |
|   `value` | `variant` |   Yes    |                            |

Example:

```YAML
type: Physics
values:
  - name: simulated
    value: false
  - name: collision-detection
    value: continuous
```

---

## `Property`

| Attribute |                                 Type                                 | Required | Description                                             |
|----------:|:--------------------------------------------------------------------:|:--------:|:--------------------------------------------------------|
|    `name` |                               `string`                               |   Yes    | Name that is unique in the scope of sibling properties. |
|    `type` | One of `string`, `int`, `float`, `bool`, `color`, `file` or `object` |   Yes    |                                                         |
|   `value` |                              `variant`                               |   Yes    | The value of the property.                              |

Note, color properties are always stored using RGBA encoding, with a `#` prefix, e.g. `"#11223344"`.

Examples:

```YAML
name: Scale
type: float
value: 0.7
```

```YAML
name: Tint
type: color
value: "#4FEB19FF"
```

```YAML
name: Associated object
type: object
value: 42
```

```YAML
name: Raw data
type: file
value: foo/bar/data.bin
```
