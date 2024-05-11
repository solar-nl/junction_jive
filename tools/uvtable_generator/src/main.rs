/*
    UV table generator for Junction Jive
*/

use std::fs::File;
use std::io::prelude::*;
use std::f64::consts::PI;
use std::f64;

const SCREEN_WIDTH: usize = 128;
const SCREEN_HEIGHT: usize = 128;
const SPRITE_WIDTH: usize = 128;
const SPRITE_HEIGHT: usize = 128;
const QUADRANT_SIZE: usize = 64;
const TILE_WIDTH: usize = 8;
const TILE_HEIGHT: usize = 8;
const SKIP_ROWS: usize = 8;
const SKIP_TILES: usize = 8;

fn main() {
    let mut x_res = 320;
    let mut y_res = 256;

    let uv_table = "tunnel";
    let tiled = false;

    if tiled == true {
        x_res = 128;
        y_res = 128;
    }

    let mut buffer: Vec<u16> = Vec::new();
    let infix = if tiled == true { "_tiled" } else { "_fb" };

    print!("const unsigned short lut_{}_size = {:?};\n", uv_table, x_res * y_res);
    print!("const unsigned short lut_{}{}[] = {{\n", uv_table, infix); 

    for j in 0..y_res {
        let mut y_dist = j - (y_res / 2);
        for i in 0..x_res {
            let mut x_dist = i - (x_res / 2);

            let distance = f64::sqrt((x_dist * x_dist + y_dist * y_dist) as f64) as i32;
            let angle = (f64::atan2(x_dist as f64, y_dist as f64) * 64.0) as i32;

            let mut u = angle;
            let mut v = distance;

            match uv_table {
                "tunnel" => {

                },
                "swirl" => {
                    u = (((-2 * angle) as f64) / PI) as i32 + distance;
                    v = f64::powf(distance as f64, 1.2) as i32;            
                },
                "ball" => {
                    if x_dist == 0 { x_dist = 1; }
                    if y_dist == 0 { y_dist = 1; }
                    let o = (0.0, 0.0, 0.5);
                    let aspect = x_res as f64 / y_res as f64;
                    let d = (i as f64 / x_res as f64 * 2.0 - 1.0, j as f64 / y_res as f64 / aspect * 2.0 - 1.0, 0.0);
                    let c = (d.0 - o.0, d.1 - o.1, d.2 - o.2);
                
                    let r = c.0 * c.0 + c.1 * c.1 + c.2 * c.2;
                
                    let theta = (-c.1 / r).acos() * 1.0;
                    let phi = (c.0 / -c.2).atan() * 2.0;
                
                    let u = (2 * (255.0 * (0.5 + phi / (f64::consts::PI * 2.0))) as i32) & 0xff;
                    let v = (2 * (255.0 * (theta / f64::consts::PI)) as i32) & 0xff;
                },
                _ => println!("Invalid effect ID"),
            }
            
            let two_bytes = ((u << 8) + v) & 0xFFFF;
            buffer.push(two_bytes as u16);
        }
    }

    if tiled == true {
        let mut top_left_quadrant: Vec<u16> = vec![0; QUADRANT_SIZE * QUADRANT_SIZE];

        for y in 0..QUADRANT_SIZE {
            for x in 0..QUADRANT_SIZE {
                let source_index = y * SCREEN_WIDTH + x;
                let dest_index = y * QUADRANT_SIZE + x;
                top_left_quadrant[dest_index] = buffer[source_index];
            }
        }

        let mut tiles: Vec<u16> = Vec::new();

        for y in (0..QUADRANT_SIZE).step_by(TILE_HEIGHT) {
            for x in (0..QUADRANT_SIZE).step_by(TILE_WIDTH) {
                for j in 0..TILE_HEIGHT {
                    for i in 0..TILE_WIDTH {
                        let pixel_index = (y + j) * QUADRANT_SIZE + (x + i);
                        let pixel = top_left_quadrant[pixel_index];
                        print!("{:?}, ", pixel);
                    }
                }
            }
            print!("\n");
        }

        for y in 0..QUADRANT_SIZE {
            for x in 0..QUADRANT_SIZE {
                let source_index = y * SCREEN_WIDTH + (x + QUADRANT_SIZE);
                let dest_index = y * QUADRANT_SIZE + x;
                top_left_quadrant[dest_index] = buffer[source_index];
            }
        }

        for y in (0..QUADRANT_SIZE).step_by(TILE_HEIGHT) {
            for x in (0..QUADRANT_SIZE).step_by(TILE_WIDTH) {
                for j in 0..TILE_HEIGHT {
                    for i in 0..TILE_WIDTH {
                        let pixel_index = (y + j) * QUADRANT_SIZE + (x + i);
                        let pixel = top_left_quadrant[pixel_index];
                        print!("{:?}, ", pixel);
                    }
                }
            }
            print!("\n");
        }

        for y in 0..QUADRANT_SIZE {
            for x in 0..QUADRANT_SIZE {
                let source_index = (y + QUADRANT_SIZE) * SCREEN_WIDTH + x;
                let dest_index = y * QUADRANT_SIZE + x;
                top_left_quadrant[dest_index] = buffer[source_index];
            }
        }

        for y in (0..QUADRANT_SIZE).step_by(TILE_HEIGHT) {
            for x in (0..QUADRANT_SIZE).step_by(TILE_WIDTH) {

                for j in 0..TILE_HEIGHT {
                    for i in 0..TILE_WIDTH {
                        let pixel_index = (y + j) * QUADRANT_SIZE + (x + i);
                        let pixel = top_left_quadrant[pixel_index];
                        print!("{:?}, ", pixel);
                    }
                }
            }
            print!("\n");
        }

        for y in 0..QUADRANT_SIZE {
            for x in 0..QUADRANT_SIZE {
                let source_index = (y + QUADRANT_SIZE) * SCREEN_WIDTH + x + QUADRANT_SIZE;
                let dest_index = y * QUADRANT_SIZE + x;
                top_left_quadrant[dest_index] = buffer[source_index];
            }
        }

        for y in (0..QUADRANT_SIZE).step_by(TILE_HEIGHT) {
            for x in (0..QUADRANT_SIZE).step_by(TILE_WIDTH) {
                for j in 0..TILE_HEIGHT {
                    for i in 0..TILE_WIDTH {
                        let pixel_index = (y + j) * QUADRANT_SIZE + (x + i);
                        let pixel = top_left_quadrant[pixel_index];
                        print!("{:?}, ", pixel);
                    }
                }
            }
            print!("\n");
        }
    }
    else {
        for i in 0..buffer.len() {
            print!("{:?}, ", buffer[i]);
            if ((i & 31) == 0) {
                print!("\n");
            }
        }
    }
    print!("}};");
}
