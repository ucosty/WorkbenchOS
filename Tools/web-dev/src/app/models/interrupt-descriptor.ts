/*
 * WorkbenchOS
 * Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

export class InterruptDescriptor {
    public descriptor_privilege_level: number | undefined;
    public offset: string | undefined;
    public present: number | undefined;
    public segment_selector: number | undefined;
    public type: number | undefined;
}