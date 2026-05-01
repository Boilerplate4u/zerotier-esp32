/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * (c) ZeroTier, Inc.
 * https://www.zerotier.com/
 */

use thiserror::Error;

#[derive(Error, Debug)]
pub enum ZeroIDCError {
    #[error(transparent)]
    DiscoveryError(#[from] openidconnect::DiscoveryError<openidconnect::reqwest::Error<reqwest::Error>>),

    #[error(transparent)]
    ParseError(#[from] url::ParseError),
}

#[derive(Error, Debug)]
#[error("SSO Exchange Error: {message:}")]
pub struct SSOExchangeError {
    message: String,
}

impl SSOExchangeError {
    pub fn new(message: String) -> Self {
        SSOExchangeError { message }
    }
}
