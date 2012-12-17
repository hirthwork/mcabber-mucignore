/*
 * mucignore.c              -- mcabber ignore MUC user plugin
 *
 * Copyright (C) 2012 Dmitry Potapov <potapov.d@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <mcabber/modules.h>
#include <mcabber/hooks.h>
#include <mcabber/logprint.h>
#include <mcabber/settings.h>

static GRegex* mucignore;
static guint mucignore_msg_in_hid;

static guint mucignore_handle_message(const gchar *hookname, hk_arg_t *args,
    gpointer userdata)
{
    const gchar* bjid = 0;
    const gchar* resource = 0;
    const gchar* groupchat = 0;
    gchar* fullname = 0;
    guint result = HOOK_HANDLER_RESULT_ALLOW_MORE_HANDLERS;

    for (; args->name; ++args) {
        if (!bjid && !g_strcmp0(args->name, "jid")) {
            bjid = args->value;
        } else if (!resource && !g_strcmp0(args->name, "resource")) {
            resource = args->value;
        } else if (!groupchat && !g_strcmp0(args->name, "groupchat")) {
            groupchat = args->value;
        }
    }

    if (groupchat && bjid && resource && !g_strcmp0(groupchat, "true")) {
        fullname = g_strdup_printf("%s/%s", bjid, resource);
        if (g_regex_match(mucignore, fullname, 0, NULL)) {
            scr_LogPrint(LPRINT_NORMAL, "Message from %s ignored", fullname);
            result = HOOK_HANDLER_RESULT_NO_MORE_HANDLER_DROP_DATA;
        }
    }
    g_free(fullname);
    return result;
}

static void mucignore_init()
{
    const gchar* ignore = settings_opt_get("mucignore");
    if (ignore) {
        mucignore = g_regex_new(ignore,
            G_REGEX_OPTIMIZE|G_REGEX_ANCHORED, G_REGEX_MATCH_ANCHORED, NULL);
        mucignore_msg_in_hid = hk_add_handler(mucignore_handle_message,
            HOOK_PRE_MESSAGE_IN, G_PRIORITY_HIGH, NULL);
    } else {
        mucignore = NULL;
    }
}

static void mucignore_uninit()
{
    if (mucignore) {
        g_regex_unref(mucignore);
        hk_del_handler(HOOK_PRE_MESSAGE_IN, mucignore_msg_in_hid);
    }
}

/* Module description */
module_info_t info_mucignore = {
        .branch         = MCABBER_BRANCH,
        .api            = MCABBER_API_VERSION,
        .version        = "0.01",
        .description    = "MUC ignore plugin",
        .requires       = NULL,
        .init           = mucignore_init,
        .uninit         = mucignore_uninit,
        .next           = NULL,
};

