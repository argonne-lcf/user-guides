import pandas as pd
import re
import datetime


base_github_url = 'https://github.com/argonne-lcf/user-guides/blob/main/docs/'
base_doc_url = 'https://docs.alcf.anl.gov/'


def split_git_log(l, delim='\|\|', file_path=None):
    splits = l.split(delim)
    fullmsg_stat = re.split(r'\n [^\n]* \| ', splits[-1])
    try:
        fullmsg, stat = fullmsg_stat[0].strip(), int(fullmsg_stat[-1].split(' ')[0])
    except (IndexError, ValueError) as e:
        fullmsg, stat = fullmsg_stat, 0
    return splits[:-1] + [fullmsg, stat]


def create_df_commits(commits_output, d='\|\|', s='__\|__'):
    # Extract author names from commit lines
    commits_parsed = [split_git_log(l, delim=d) for l in commits_output.split(s)[1:]]
    columns = ['file_path', 'commit_hash', 'message_title', 'author_name', 'author_email', 'relative_date', 'commit_date', 'full_message', 'num_edits']
    df_comm = pd.DataFrame(commits_parsed, columns=columns)
    df_comm['commit_hash'] = df_comm['commit_hash'].str[:7]
    df_comm['commit_date'] = pd.to_datetime(df_comm['commit_date'], format="%a %b %d %H:%M:%S %Y %z", utc=True).dt.tz_localize(None)
    return df_comm


def process_file_commits(df_comm):
    df = pd.DataFrame([[]])

    # edits and activity
    edits_total, edits_this_year = agg_col_ever_and_this_year(df_comm, col='num_edits', func='sum')
    df['edits_total'] = edits_total
    df['edits_this_year'] = edits_this_year
    commits_total, commits_this_year = agg_col_ever_and_this_year(df_comm, col='num_edits', func='count')
    df['commits_total'] = commits_total
    df['commits_this_year'] = commits_this_year
    df['date_last_commit'] = df_comm['commit_date'].max()

    # authors
    user_col = 'author_email'
    top4_authors_w_most_edits = sort_authors_by_number_of_edits(df_comm, in_the_last_year=False, user_col=user_col)[:4].tolist()
    df["top4_authors_w_most_edits"] = [top4_authors_w_most_edits]
    df["author_w_most_edits"] = top4_authors_w_most_edits[0]
    try:
        author_w_most_edits = sort_authors_by_number_of_edits(df_comm, in_the_last_year=True, user_col=user_col)[0]
    except IndexError:
        author_w_most_edits = None
    df["author_w_most_edits_this_year"] = author_w_most_edits

    # system and page name
    file_path = df_comm['file_path'].values[0].removeprefix('./docs/')
    df['system'] = file_path.split('/')[0]
    df['name'] = file_path.split('/')[-1]
    # add github url
    df['github_url'] = base_github_url + file_path
    df['url'] = df['github_url'].str.replace(base_github_url, base_doc_url)
    df['url'] = df['url'].str.replace(".md$", "/", regex=True).values
    return df


def sort_authors_by_number_of_edits(df_comm, in_the_last_year=True, user_col='author_name'):
    c = df_comm
    if in_the_last_year:
        one_year_ago = datetime.datetime.today() - datetime.timedelta(days=365)
        authors_by_num_edits = c[c['commit_date'] > one_year_ago].groupby(user_col)['num_edits'].sum()
    else:
        authors_by_num_edits = c.groupby(user_col)['num_edits'].sum()
    return authors_by_num_edits.sort_values(ascending=False).index.values


def agg_col_ever_and_this_year(df_comm, col, func='sum'):
    c = df_comm
    one_year_ago = datetime.datetime.today() - datetime.timedelta(days=365)
    tot_this_year = c[c['commit_date'] > one_year_ago][col].agg(func)
    tot_ever = c[col].agg(func)
    return tot_ever, tot_this_year


def main(commit_history_path, d='||', s='__|__', file_delim='_/^\_'):
    with open(commit_history_path, 'r', encoding='utf-8') as file:
        commit_history = file.read().split(file_delim)

    log_entries = []
    for commits_output in commit_history:
        df_comm = create_df_commits(commits_output, d=d, s=s)
        df = process_file_commits(df_comm)
        log_entries.append(df)

    _df = pd.concat(log_entries)
    _df = _df.sort_values(['system', 'date_last_commit'], ascending=[True, False]).reset_index(drop=True)
    _df.to_csv('commits_summary.csv', index=False)


if __name__ == '__main__':
    main('commit_history.txt')
