use thiserror::Error;

/// Errors calling underlying functions.
#[derive(Debug, Error)]
pub enum ExecutionError {
    #[error("File is {0} bytes exceeding the value a u32 size can hold")]
    FileTooLong(usize),
}
