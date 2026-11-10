-- SPDX-FileCopyrightText: © 2026 Adrian Johnston.
-- SPDX-License-Identifier: MIT
-- This file is licensed under the terms of the LICENSE.md file.

function Meta(meta)
  local source = meta['tab-title'] or meta.title
  if source then
    local title = (pandoc.utils.stringify(source):gsub('%s+', ' '))
    meta.pagetitle = title
    meta['title-meta'] = title
  end
  return meta
end
