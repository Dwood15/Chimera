extern crate hyper;
extern crate futures;
extern crate tokio_core;

use std::sync::{Arc, Mutex};
use std::io::Write;
use futures::{Future, Stream};
use hyper::Client;
use tokio_core::reactor::Core;
use std::thread;

pub struct CheckStatus {
    data : Arc<Mutex<Vec<u8>>>
}

#[no_mangle]
pub unsafe extern "C" fn latest_update(status : *mut CheckStatus) -> *const u8 {
    let l = (*status).data.clone();
    let r = l.try_lock();
    if let Ok(result) = r {
        if result.len() == 0 {
            0 as *const _
        }
        else {
            result[..].as_ptr()
        }
    }
    else {
        0 as *const _
    }
}

#[no_mangle]
pub extern "C" fn check_for_updates() -> *mut CheckStatus {
    let cs = CheckStatus {
        data : Arc::new(Mutex::new(Vec::new()))
    };
    let ret = Box::into_raw(Box::new(cs));
    let r = ret as usize;

    println!("Checking for updates...");

    thread::spawn( move || {
        println!("New thread!");
        let mut core = Core::new().unwrap();
        let client = Client::new(&core.handle());
        let url = "http://chimera.opencarnage.net/update.txt".parse().unwrap();
        let work = client.get(url).map(|res| {
            println!("Got a result.");
            let mut v : Vec<u8> = Vec::new();
            let chunks_m = res.body().collect().wait();
            if let Ok(chunks) = chunks_m {
                for i in chunks {
                    v.write_all(&i).unwrap();
                    if v.len() > 16 {
                        break;
                    }
                }
            }
            else {
                v.write_all(b"failed").unwrap();
            }
            println!("{}",String::from_utf8(v.clone()).unwrap());
            v.push(0);
            unsafe {
                let lock = (*(r as *mut CheckStatus)).data.clone();
                let mut locked = lock.lock().unwrap();
                *locked = v;
            }
        });
        println!("Running core...");
        match core.run(work) {
            Ok(_) => println!("OK!"),
            Err(n) => println!("{}", n)
        };
        println!("Core ran!");
    });

    return ret;
}
