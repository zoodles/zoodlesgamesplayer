/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package org.mozilla.gecko.mozglue.generatorannotations;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;

/**
 * Handy shortcut annotation. Functionally equivalent to tagging every member individually with default
 * settings.
 */
@Retention(RetentionPolicy.RUNTIME)
public @interface WrapEntireClassForJNI {}
